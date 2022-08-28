//
// Created by Cu1 on 2022/8/4.
//

#ifndef ZEPHYR_ALLOCATOR_H
#define ZEPHYR_ALLOCATOR_H

#include "pool_allocator.h"
#include "construct.h"

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

    template <typename ... Args>
    static void construct(T* ptr, Args&& ...args);

    static void destroy(T* ptr);
    static void destroy(T* first, T* last);

};

template <typename T>
T* allocator<T>::allocate() {
    return static_cast<T*>(pool_allocator::allocate(sizeof(T)));
}

template <typename T>
T* allocator<T>::allocate(size_type n) {
    if (n == 0)
        return nullptr;
    return static_cast<T*>(pool_allocator::allocate(n * sizeof(T)));
}

template <typename T>
void allocator<T>::deallocate(T* ptr) {
    if (ptr == nullptr)
        return ;
    pool_allocator::deallocate(p, sizeof(T));
}

template <typename T>
void allocator<T>::deallocate(T* ptr, size_type n) {
    if (ptr == nullptr || n == 0)
        return ;
    pool_allocator::deallocate(p, n * sizeof(T));
}

template <typename T>
void allocator<T>::construct(T* ptr) {
    zephyr::construct(ptr);
}

template <typename T>
void allocator<T>::construct(T* ptr, const T& value) {
    zephyr::construct(ptr, value);
}

template <typename T>
void allocator<T>::construct(T* ptr, T&& value) {
    zephyr::construct(ptr, std::move(value));
}

template <typename T>
template <typename ...Args>
void allocator<T>::construct(T* ptr, Args&& ...args) {
    zephyr::construct(ptr, std::forward<Args>(args)...)
}

template <typename T>
void allocator<T>::destroy(T* ptr) {
    zephyr::destroy(ptr);
}

template <typename T>
void allocator<T>::destroy(T* first, T* last) {
    zephyr::destroy(first, last);
}






}


#endif //ZEPHYR_ALLOCATOR_H
