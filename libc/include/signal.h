/* Copyright (c) 2017, 2019, 2020, 2022, 2024 Dennis Wölfing
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

/* libc/include/signal.
 * Signals.
 */

#ifndef _SIGNAL_H
#define _SIGNAL_H

#include <sys/cdefs.h>
#include <dennix/signal.h>
#if __USE_DENNIX || __USE_POSIX
#  define __need_pid_t
#  define __need_pthread_t
#  define __need_pthread_attr_t
#  define __need_size_t
#  define __need_uid_t
#  include <bits/types.h>
#  include <dennix/sigaction.h>
#  include <dennix/siginfo.h>
#  include <dennix/signalcodes.h>
#  include <dennix/sigset.h>
#  include <dennix/timespec.h>
#  include <dennix/ucontext.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef __SIG_ATOMIC_TYPE__ sig_atomic_t;

int raise(int);
void (*signal(int, void(*)(int)))(int);

#if __USE_DENNIX || __USE_POSIX
int kill(pid_t, int);
int pthread_sigmask(int, const sigset_t* __restrict, sigset_t* __restrict);
int sigaction(int, const struct sigaction* __restrict,
        struct sigaction* __restrict);
int sigaddset(sigset_t*, int);
int sigdelset(sigset_t*, int);
int sigemptyset(sigset_t*);
int sigfillset(sigset_t*);
int sigismember(const sigset_t*, int);
int sigprocmask(int, const sigset_t* __restrict, sigset_t* __restrict);
int sigtimedwait(const sigset_t* __restrict, siginfo_t* __restrict,
        const struct timespec* __restrict);
int sigwait(const sigset_t* __restrict, int* __restrict);
int sigwaitinfo(const sigset_t* __restrict, siginfo_t* __restrict);
#endif

#if __USE_DENNIX || __USE_POSIX >= 202405L
#  define SIG2STR_MAX 8

int sig2str(int, char*);
int str2sig(const char* __restrict, int* __restrict);
#endif

#if __USE_DENNIX
#  define NSIG _NSIG
#endif

#ifdef __cplusplus
}
#endif

#endif
