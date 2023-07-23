/* Copyright (c) 2018, 2019, 2020, 2021, 2023 Dennis Wölfing
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* kernel/src/pipe.cpp
 * Pipes.
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>
#include <dennix/poll.h>
#include <dennix/kernel/pipe.h>
#include <dennix/kernel/signal.h>
#include <dennix/kernel/thread.h>

class PipeVnode::Endpoint : public Vnode {
public:
    Endpoint(const Reference<PipeVnode>& pipe)
            : Vnode(S_IFIFO | S_IRUSR | S_IWUSR, 0), pipe(pipe) {}
    int stat(struct stat* result) override;
protected:
    Reference<PipeVnode> pipe;
};

class PipeVnode::ReadEnd : public Endpoint {
public:
    ReadEnd(const Reference<PipeVnode>& pipe) : Endpoint(pipe) {}
    ~ReadEnd();
    NOT_COPYABLE(ReadEnd);
    NOT_MOVABLE(ReadEnd);

    short poll() override;
    ssize_t read(void* buffer, size_t size, int flags) override;
};

class PipeVnode::WriteEnd : public Endpoint {
public:
    WriteEnd(const Reference<PipeVnode>& pipe) : Endpoint(pipe) {}
    ~WriteEnd();
    NOT_COPYABLE(WriteEnd);
    NOT_MOVABLE(WriteEnd);

    short poll() override;
    ssize_t write(const void* buffer, size_t size, int flags) override;
};

PipeVnode::PipeVnode(Reference<Vnode>& readPipe, Reference<Vnode>& writePipe)
        : Vnode(S_IFIFO | S_IRUSR | S_IWUSR, 0),
        circularBuffer(pipeBuffer, sizeof(pipeBuffer)) {
    readEnd = new ReadEnd(this);
    if (!readEnd) FAIL_CONSTRUCTOR;
    writeEnd = new WriteEnd(this);
    if (!writeEnd) {
        delete readEnd;
        readEnd = nullptr;
        FAIL_CONSTRUCTOR;
    }

    readCond = KTHREAD_COND_INITIALIZER;
    writeCond = KTHREAD_COND_INITIALIZER;
    readPipe = readEnd;
    writePipe = writeEnd;
}

PipeVnode::~PipeVnode() {
    assert(!readEnd);
    assert(!writeEnd);
}

int PipeVnode::Endpoint::stat(struct stat* result) {
    return pipe->stat(result);
}

short PipeVnode::ReadEnd::poll() {
    return pipe->poll() & (POLLIN | POLLRDNORM | POLLHUP);
}

ssize_t PipeVnode::ReadEnd::read(void* buffer, size_t size, int flags) {
    return pipe->read(buffer, size, flags);
}

PipeVnode::ReadEnd::~ReadEnd() {
    AutoLock lock(&pipe->mutex);
    pipe->readEnd = nullptr;
    kthread_cond_broadcast(&pipe->writeCond);
}

short PipeVnode::WriteEnd::poll() {
    return pipe->poll() & (POLLOUT | POLLWRNORM | POLLHUP);
}

ssize_t PipeVnode::WriteEnd::write(const void* buffer, size_t size, int flags) {
    return pipe->write(buffer, size, flags);
}

PipeVnode::WriteEnd::~WriteEnd() {
    AutoLock lock(&pipe->mutex);
    pipe->writeEnd = nullptr;
    kthread_cond_broadcast(&pipe->readCond);
}

short PipeVnode::poll() {
    AutoLock lock(&mutex);
    short result = 0;
    if (circularBuffer.bytesAvailable()) result |= POLLIN | POLLRDNORM;
    if (readEnd && circularBuffer.spaceAvailable()) {
        result |= POLLOUT | POLLWRNORM;
    }
    if (!readEnd || !writeEnd) result |= POLLHUP;
    return result;
}

ssize_t PipeVnode::read(void* buffer, size_t size, int flags) {
    if (size == 0) return 0;
    AutoLock lock(&mutex);

    while (circularBuffer.bytesAvailable() == 0) {
        if (!writeEnd) return 0;

        if (flags & O_NONBLOCK) {
            errno = EAGAIN;
            return -1;
        }

        if (kthread_cond_sigwait(&readCond, &mutex) == EINTR) {
            errno = EINTR;
            return -1;
        }
    }

    size_t bytesRead = circularBuffer.read(buffer, size);
    kthread_cond_broadcast(&writeCond);
    updateTimestamps(true, false, false);
    return bytesRead;
}

ssize_t PipeVnode::write(const void* buffer, size_t size, int flags) {
    if (size == 0) return 0;
    AutoLock lock(&mutex);

    if (size <= PIPE_BUF) {
        while (circularBuffer.spaceAvailable() < size && readEnd) {
            if (flags & O_NONBLOCK) {
                errno = EAGAIN;
                return -1;
            }

            if (kthread_cond_sigwait(&writeCond, &mutex) == EINTR) {
                errno = EINTR;
                return -1;
            }
        }
    }

    const char* buf = (const char*) buffer;
    size_t written = 0;

    while (written < size) {
        while (circularBuffer.spaceAvailable() == 0 && readEnd) {
            if (flags & O_NONBLOCK) {
                if (written) {
                    updateTimestamps(false, true, true);
                    return written;
                }
                errno = EAGAIN;
                return -1;
            }

            if (kthread_cond_sigwait(&writeCond, &mutex) == EINTR) {
                if (written) {
                    updateTimestamps(false, true, true);
                    return written;
                }
                errno = EINTR;
                return -1;
            }
        }

        if (!readEnd) {
            siginfo_t siginfo = {};
            siginfo.si_signo = SIGPIPE;
            siginfo.si_code = SI_KERNEL;
            Thread::current()->raiseSignal(siginfo);
            errno = EPIPE;
            return -1;
        }

        written += circularBuffer.write(buf + written, size - written);
        kthread_cond_broadcast(&readCond);
    }

    updateTimestamps(false, true, true);
    return written;
}
