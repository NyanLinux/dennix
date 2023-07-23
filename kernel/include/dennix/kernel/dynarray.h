/* Copyright (c) 2017, 2018, 2023 Dennis Wölfing
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

/* kernel/include/dennix/kernel/dynarray.h
 * Dynamically resizable arrays.
 */

#ifndef KERNEL_DYNARRAY_H
#define KERNEL_DYNARRAY_H

#include <assert.h>
#include <stdlib.h>
#include <dennix/kernel/kernel.h>

template <typename T, typename TSize = size_t>
class DynamicArray {
public:
    DynamicArray() {
        allocatedSize = 0;
        buffer = nullptr;
    }

    ~DynamicArray() {
        for (TSize i = 0; i < allocatedSize; i++) {
            buffer[i].~T();
        }
        free(buffer);
    }

    NOT_COPYABLE(DynamicArray);
    NOT_MOVABLE(DynamicArray);

    TSize add(const T& obj) {
        return addAt(0, obj);
    }

    TSize addAt(TSize index, const T& obj) {
        TSize i;
        for (i = index; i < allocatedSize; i++) {
            if (!buffer[i]) {
                buffer[i] = obj;
                return i;
            }
        }

        TSize newSize;
        if (__builtin_add_overflow(i, 1, &newSize)) {
            return (TSize) -1;
        }
        if (!resize(newSize)) return (TSize) -1;
        buffer[i] = obj;
        return i;
    }

    void clear() {
        for (TSize i = 0; i < allocatedSize; i++) {
            buffer[i].~T();
        }
        free(buffer);
        allocatedSize = 0;
        buffer = nullptr;
    }

    TSize insert(TSize index, const T& obj) {
        assert(index >= 0);
        if (index >= allocatedSize) {
            TSize newSize;
            if (__builtin_add_overflow(index, 1, &newSize)) {
                return (TSize) -1;
            }
            if (!resize(newSize)) return (TSize) -1;
        }

        buffer[index] = obj;
        return index;
    }

    TSize next(TSize index) {
        for (TSize i = index + 1; i < allocatedSize; i++) {
            if (buffer[i]) return i;
        }
        return (TSize) -1;
    }

    void remove(TSize index) {
        buffer[index] = T();
    }

    bool resize(TSize size) {
        assert(size > allocatedSize);
        T* newBuffer = (T*) reallocarray(buffer, (size_t) size, sizeof(T));
        if (!newBuffer) return false;
        buffer = newBuffer;
        for (TSize i = allocatedSize; i < size; i++) {
            new (&buffer[i]) T();
        }
        allocatedSize = size;
        return true;
    }

    T& operator[](TSize index) {
        assert(index >= 0 && index < allocatedSize);
        return buffer[index];
    }

public:
    TSize allocatedSize;
private:
    T* buffer;
};

#endif
