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

/* kernel/include/dennix/kernel/pipe.h
 * Pipes.
 */

#ifndef KERNEL_PIPE_H
#define KERNEL_PIPE_H

#include <dennix/kernel/circularbuffer.h>
#include <dennix/kernel/vnode.h>

class PipeVnode : public Vnode, public ConstructorMayFail {
private:
    // The pipe needs to reference count the read and write ends separately.
    // Thus we create classes for both ends. FileDescriptions should only be
    // opened for these two ends, but not for the pipe itself.
    class Endpoint;
    class ReadEnd;
    class WriteEnd;
public:
    PipeVnode(Reference<Vnode>& readPipe, Reference<Vnode>& writePipe);
    NOT_COPYABLE(PipeVnode);
    NOT_MOVABLE(PipeVnode);
    virtual ~PipeVnode();

    short poll() override;
    ssize_t read(void* buffer, size_t size, int flags) override;
    ssize_t write(const void* buffer, size_t size, int flags) override;
private:
    Vnode* readEnd;
    Vnode* writeEnd;
    char pipeBuffer[PIPE_BUF];
    CircularBuffer circularBuffer;
    kthread_cond_t readCond;
    kthread_cond_t writeCond;
};

#endif
