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

struct test_node {
    test_node* data1;
    test_node* data2;
    double data3;
};

test_node* p[1000005];

void alloc_test() {

#define MAX_NEW 1000000

    count_new = 0;
    times_count = 0;

    std::cout << "test_node size = " << sizeof(test_node) << std::endl;

    for (int i = 0; i < MAX_NEW; i++) {
        p[i] = new test_node;
    }
    std::cout << "new result: times_count = " << times_count << " count_new = " << count_new << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    count_new = 0;
    times_count = 0;
    for (int i = 0; i < MAX_NEW; i++) {
        delete p[i];
    }

    for (int i = 0; i < MAX_NEW; i++) {
        p[i] = zephyr::pool_alloc<test_node>::allocate(1);
    }
    std::cout << "zephyr::pool_alloc result: times_count = " << times_count << " count_new = " << count_new << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    for (int i = 0; i < MAX_NEW; i++) {
        zephyr::pool_alloc<test_node>::deallocate(p[i]);
    }

    times_count = 0, count_new = 0;

    for (int i = 0; i < MAX_NEW; i++) {
        p[i] = zephyr::loki_alloc<test_node>::allocate();
    }

    //p[0] = loki_alloc<test_node>::allocate();
    std::cout << "zephyr::loki_alloc: times_count = " << times_count << " count_new = " << count_new << std::endl;
    std::cout << "-------------------------------------------------------------------------" << std::endl;

    std::cout << "before" << std::endl;
    for (int i = 0; i < MAX_NEW; i++)
        zephyr::loki_alloc<test_node>::deallocate(p[i]);
    std::cout << "after" << std::endl;
}

} // namespace zephyr::alloc_test

} // namespace zephyr