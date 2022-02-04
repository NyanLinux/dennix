/* Copyright (c) 2016, 2018, 2019, 2020, 2022 Dennis Wölfing
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

/* libc/include/sys/types.h
 * Data types.
 */

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

#include <sys/cdefs.h>

#define __need_blkcnt_t
#define __need_blksize_t
#define __need_clock_t
#define __need_clockid_t
#define __need_dev_t
#define __need_fsblkcnt_t
#define __need_fsfilcnt_t
#define __need_gid_t
#define __need_id_t
#define __need_ino_t
#define __need_mode_t
#define __need_nlink_t
#define __need_off_t
#define __need_pid_t
#define __need_reclen_t
#define __need_size_t
#define __need_ssize_t
#define __need_suseconds_t
#define __need_time_t
#define __need_timer_t
#define __need_uid_t
#include <bits/types.h>
#include <bits/pthread.h>

#endif
