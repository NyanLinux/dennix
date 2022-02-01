/* Copyright (c) 2018, 2019, 2022 Dennis Wölfing
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

/* libc/src/stdio/ungetc_unlocked.c
 * Push back byte to file stream. (called from C89)
 */

#define fflush_unlocked __fflush_unlocked
#include "FILE.h"

int __ungetc_unlocked(int c, FILE* file) {
    if (c == EOF) return EOF;

    if (fileWasWritten(file) && fflush_unlocked(file) == EOF) {
        return EOF;
    }

    if (file->readPosition == 0) return EOF;
    file->buffer[--file->readPosition] = (unsigned char) c;
    file->flags &= ~FILE_FLAG_EOF;

    return (unsigned char) c;
}
__weak_alias(__ungetc_unlocked, ungetc_unlocked);
