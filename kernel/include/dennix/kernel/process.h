/* Copyright (c) 2016, 2017, 2018 Dennis Wölfing
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

/* kernel/include/dennix/kernel/process.h
 * Process class.
 */

#ifndef KERNEL_PROCESS_H
#define KERNEL_PROCESS_H

#include <sys/types.h>
#include <dennix/fork.h>
#include <dennix/kernel/addressspace.h>
#include <dennix/kernel/dynarray.h>
#include <dennix/kernel/filedescription.h>
#include <dennix/kernel/thread.h>

struct FdTableEntry {
    Reference<FileDescription> descr;
    int flags;

    operator bool() { return descr; }
};

class Process {
public:
    Process();
    ~Process();
    int addFileDescriptor(const Reference<FileDescription>& descr, int flags);
    int close(int fd);
    int dup3(int fd1, int fd2, int flags);
    void exit(int status);
    int execute(const Reference<Vnode>& vnode, char* const argv[],
            char* const envp[]);
    int fcntl(int fd, int cmd, int param);
    Reference<FileDescription> getFd(int fd);
    void raiseSignal(siginfo_t siginfo);
    Process* regfork(int flags, struct regfork* registers);
    void terminateBySignal(siginfo_t siginfo);
    Process* waitpid(pid_t pid, int flags);
private:
    void terminate();
public:
    AddressSpace* addressSpace;
    Clock cpuClock;
    Reference<FileDescription> cwdFd;
    Thread mainThread;
    pid_t pid;
    Reference<FileDescription> rootFd;
    struct sigaction sigactions[NSIG];
    vaddr_t sigreturn;
    siginfo_t terminationStatus;
    mode_t umask;
private:
    kthread_mutex_t childrenMutex;
    DynamicArray<FdTableEntry, int> fdTable;
    Process* firstChild;
    Process* nextChild;
    Process* parent;
    Process* prevChild;
    bool terminated;
public:
    static bool addProcess(Process* process);
    static Process* current() { return Thread::current()->process; }
    static Process* initProcess;
private:
    static int copyArguments(char* const argv[], char* const envp[],
            char**& newArgv, char**& newEnvp, AddressSpace* newAddressSpace);
    static uintptr_t loadELF(uintptr_t elf, AddressSpace* newAddressSpace);
};

#endif
