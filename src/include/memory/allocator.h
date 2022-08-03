//
// Created by Cu1 on 2022/8/4.
//

#ifndef ZEPHYR_ALLOCATOR_H
#define ZEPHYR_ALLOCATOR_H

#include "pool_alloc.hpp"

// 这个头文件包含一个模板类 allocator，用于管理内存的分配、释放，对象的构造、析构

namespace zephyr
{

// 模板类：allocator
// 模板函数代表数据类型
template <typename T>
class allocator {

public:
    typedef T            value_type;
    typedef T*           pointer;
    typedef const T*     const_pointer;
    typedef T&           reference;
    typedef const T&     const_reference;
    typedef size_t       size_type;
    typedef ptrdiff_t    difference_type;

public:
    static T*   allocate();
    static T*   allocate(size_type n);

    static void deallocate(T* ptr);
    static void deallocate(T* ptr, size_type n);

    static void construct(T* ptr);
    static void construct(T* ptr, const T& value);
    static void construct(T* ptr, T&& value);

    template <class... Args>
    static void construct(T* ptr, Args&& ...args);

    static void destroy(T* ptr);
    static void destroy(T* first, T* last);

};




}


#endif //ZEPHYR_ALLOCATOR_H
