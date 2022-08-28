//
// Created by Cu1 on 2022/8/2.
//

#include <iostream>

#include "../src/include/memory/pool_allocator.h"
#include "../src/include/memory/loki_allocator.h"

long long count_new;
long long times_count;

void* operator new(size_t size) {
    times_count++;
    count_new += size;
    //std::cout << "times_count = " << times_count << " zephyr globe new = " << size << std::endl;
    return std::malloc(size);
}

namespace zephyr
{

namespace alloc_test
{

void* p[1000005];

void alloc_test() {

#define MAX_NEW 1000000

    count_new = 0;
    times_count = 0;
    for (int i = 0; i < MAX_NEW; i++) {
        p[i] = ::operator new(16);
    }
    std::cout << "::times_count = " << times_count << " ::count_new = " << count_new << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    count_new = 0;
    times_count = 0;
    for (int i = 0; i < MAX_NEW; i++) {
        ::operator delete(p[i]);
    }

    for (int i = 0; i < MAX_NEW; i++) {
        p[i] = zephyr::pool_allocator::allocate(16);
    }
    std::cout << "::times_count = " << times_count << " ::count_new = " << count_new << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;
    for (int i = 0; i < MAX_NEW; i++) {
        zephyr::pool_allocator::deallocate(p[i], 16);
    }
}

} // namespace zephyr::alloc_test

} // namespace zephyr