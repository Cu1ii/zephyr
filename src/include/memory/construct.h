//
// Created by Cu1 on 2022/8/5.
//

#ifndef ZEPHYR_CONSTRUCT_H
#define ZEPHYR_CONSTRUCT_H

// 该头文件包含两个函数 construct, destroy
// construct: 负责对象的构造
// destroy: 负责对象的析构

#include <utility>      // std::forward
#include <iterator>

namespace zephyr
{

template <typename T>
void construct(T* ptr) {
    ::new((void*)ptr) T();
}

template <typename T1, typename T2>
void construct(T1* ptr, const T2& value) {
    ::new((void*)ptr) T1(value);
}

template <typename T, typename ...Args>
void construct(T* ptr, Args&& ...args) {
    ::new((void*)ptr) T(std::forward<Args>(args)...);
}

template <typename T>
void destroy_one(T*, std::true_type) {}

template <typename T>
void destroy_one(T* ptr, std::false_type) {
    if (ptr != nullptr)
        ptr->~T();
}

template <typename T>
void destroy(T* ptr) {
    destroy_one(ptr, std::is_trivially_destructible<T>{});
}

template <typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::true_type) {}

template <typename ForwardIter>
void destroy_cat(ForwardIter first, ForwardIter last, std::false_type) {
    for (; first != last; ++first) {
        destroy(&(*first));
    }
}

template <typename ForwardIter>
void destroy(ForwardIter first, ForwardIter last) {
    destroy_cat(first, last, std::is_trivially_destructible<
            typename std::iterator_traits<ForwardIter>::value_type>{});
}


}




#endif //ZEPHYR_CONSTRUCT_H
