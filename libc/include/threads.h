/* Copyright (c) 2022 Dennis Wölfing
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

/* libc/include/threads.h
 * Threads.
 */

#ifndef _THREADS_H
#define _THREADS_H

#include <time.h>
#include <bits/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define thread_local _Thread_local
#define ONCE_FLAG_INIT _ONCE_INIT

typedef __cond_t cnd_t;
typedef __mutex_t mtx_t;
typedef __once_t once_flag;
typedef __thread_t thrd_t;
typedef int (*thrd_start_t)(void *);

enum {
    mtx_plain = _MUTEX_NORMAL,
    mtx_recursive = _MUTEX_RECURSIVE,
    mtx_timed = _MUTEX_NORMAL,
};

enum {
    thrd_success,
    thrd_busy,
    thrd_error,
    thrd_nomem,
    thrd_timedout
};

void call_once(once_flag*, void (*)(void));
int cnd_broadcast(cnd_t*);
void cnd_destroy(cnd_t*);
int cnd_init(cnd_t*);
int cnd_signal(cnd_t*);
int cnd_timedwait(cnd_t* __restrict, mtx_t* __restrict,
        const struct timespec* __restrict);
int cnd_wait(cnd_t*, mtx_t*);
void mtx_destroy(mtx_t*);
int mtx_init(mtx_t*, int);
int mtx_lock(mtx_t*);
int mtx_timedlock(mtx_t* __restrict, const struct timespec* __restrict);
int mtx_trylock(mtx_t*);
int mtx_unlock(mtx_t*);
int thrd_create(thrd_t*, thrd_start_t, void*);
thrd_t thrd_current(void);
__noreturn void thrd_exit(int);
int thrd_join(thrd_t, int*);

#ifdef __cplusplus
}
#endif

#endif
