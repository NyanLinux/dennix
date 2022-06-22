/* Copyright (c) 2016, 2017, 2018, 2019, 2022 Dennis Wölfing
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

/* libc/src/stdio/fclose.c
 * Closes a file. (C89)
 */

#define close __close
#define pthread_mutex_lock __mutex_lock
#define pthread_mutex_unlock __mutex_unlock
#include <stdlib.h>
#include <unistd.h>
#include "FILE.h"

int fclose(FILE* file) {
    int result = 0;
    if (fflush(file) == EOF) result = EOF;

    // Remove the file from the file list.
    pthread_mutex_lock(&__fileListMutex);
    if (file == __firstFile) {
        __firstFile = file->next;
    }
    if (file->prev) {
        file->prev->next = file->next;
    }
    if (file->next) {
        file->next->prev = file->prev;
    }
    pthread_mutex_unlock(&__fileListMutex);

    if (file->fd != -1 && close(file->fd) < 0) {
        result = EOF;
    }

    if (file != stdin && file != stdout && file != stderr) {
        free(file);
    }

    return result;
}
