//
// Created by Cu1 on 2022/8/2.
//

#ifndef ZEPHYR_POOL_ALLOCATOR_H
#define ZEPHYR_POOL_ALLOCATOR_H

#include <new>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

namespace zephyr
{

struct Obj {
    Obj* free_list_next;
};

enum { Z_align = 8 };
enum { Z_max_bytes = 128 };
enum { Z_free_list_size = Z_max_bytes / Z_align };

class pool_allocator {
private:

    static char* Z_heap_start;
    static char* Z_heap_end;
    static size_t Z_heap_size;

    static Obj* Z_free_list[Z_free_list_size];

    static size_t Z_align_size_list[Z_free_list_size];

public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_size, size_t new_size);
private:
    static size_t Z_round_up(size_t bytes);
    static void*  Z_refill(size_t n);
    static size_t Z_freelist_index(size_t bytes);
    static char*  Z_chunk_alloc(size_t size, size_t& nblock);
};

char* pool_allocator::Z_heap_start = nullptr;
char* pool_allocator::Z_heap_end = nullptr;
size_t pool_allocator::Z_heap_size = 0;

Obj* pool_allocator::Z_free_list[Z_free_list_size] = {
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
};

size_t pool_allocator::Z_align_size_list[Z_free_list_size] = {
        8, 16, 24, 32, 40, 48, 56, 64,
        72, 80, 88, 96, 104, 112, 120, 128,
};

inline void* pool_allocator::allocate(size_t _size) {
    if (_size > static_cast<size_t>(Z_max_bytes))
        return ::operator new(Z_max_bytes);
    Obj*& free_list_index = Z_free_list[Z_freelist_index(_size)];
    Obj* result = free_list_index;
    if (result == nullptr) {
        void* r = Z_refill(Z_round_up(_size));
        return r;
    }
    free_list_index = result->free_list_next;
    return result;
}

inline void pool_allocator::deallocate(void* p, size_t _size) {
    if (_size > static_cast<size_t>(Z_max_bytes)) {
        ::operator delete(p);
        return;
    }
    Obj* q = (Obj*)p;
    q->free_list_next = Z_free_list[Z_freelist_index(_size)];
    Z_free_list[Z_freelist_index(_size)] = q;
}

inline void* pool_allocator::reallocate(void* p, size_t old_size, size_t new_size) {
    deallocate(p, old_size);
    return allocate(new_size);
}

inline size_t pool_allocator::Z_round_up(size_t _size) {
    size_t l = 0, r = 15, res = 0;

    while (l <= r) {
        size_t mid = (l + r) / 2;
        if (Z_align_size_list[mid] >= _size)
            res = Z_align_size_list[mid],
            r = mid - 1;
        else
            l = mid + 1;
    }
    return res;
}

inline size_t pool_allocator::Z_freelist_index(size_t _size) {
    return Z_round_up(_size) / 8 - 1;
}


void* pool_allocator::Z_refill(size_t n) {
    size_t nblock = 10;
    char* c = Z_chunk_alloc(n, nblock);
    if (nblock == 1)
        return static_cast<void*>(c);

    Obj*& free_list_index = Z_free_list[Z_freelist_index(n)];
    Obj* result = (Obj*)c;
    Obj* cur = nullptr;
    Obj* next = nullptr;
    free_list_index = next = (Obj*)(c + n);
    for (size_t i = 1; i < nblock; ++i) {
        cur = next,
        next = (Obj*)((char*) next + n);
        cur->free_list_next = (i == nblock - 1) ? nullptr : next;
    }
    return result;
}

char* pool_allocator::Z_chunk_alloc(size_t size, size_t& nblock) {
    char* result = nullptr;
    size_t need_size = size * nblock;
    size_t heap_size = Z_heap_end - Z_heap_start;
    if (heap_size >= need_size) {
        result = Z_heap_start;
        Z_heap_start += need_size;
        return result;
    }
    if (heap_size >= size) {
        nblock = heap_size / size;
        need_size = nblock * size;
        result = Z_heap_start;
        Z_heap_start += need_size;
        return result;
    }

    if (heap_size > 0) {
       ((Obj*)(Z_heap_start))->free_list_next = Z_free_list[Z_freelist_index(heap_size)];
        Z_free_list[Z_freelist_index(heap_size)] = (Obj*)(Z_heap_start);
    }
    size_t require_size = need_size * 2 * 2;
    Z_heap_start = (char*) ::operator new(require_size);
    if (Z_heap_start == nullptr) {
        Obj* p;
        for (size_t i = size; i <= Z_max_bytes; i += Z_align) {
            Obj*& free_list_index = Z_free_list[i / 8 - 1];
            p = free_list_index;
            if (p) {
                free_list_index->free_list_next = p->free_list_next;
                Z_heap_start = (char*)p;
                Z_heap_end = Z_heap_start + i;
                return Z_chunk_alloc(size, nblock);
            }
        }
        puts("out of memory");
        Z_heap_start = Z_heap_end = nullptr;
        throw std::bad_alloc();
    }
    Z_heap_end = Z_heap_start + require_size;
    Z_heap_size  += require_size;
    return Z_chunk_alloc(size, nblock);
}

}


#endif //ZEPHYR_POOL_ALLOCATOR_H
