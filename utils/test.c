/* Copyright (c) 2016, Dennis Wölfing
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

/* utils/test.c
 * Some program to test program loading.
 */

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    (void) argc; (void) argv;
    printf("Hello %s from userspace!\n", "World");

    pid_t pid = fork();

    if (pid == -1) {
        printf("fork() failed\n");
    } else if (pid == 0) {
        printf("Hello from child process!\n");
    } else {
        printf("Hello from parent process. The new process has pid %u.\n", pid);
    }

    return 42;
}
