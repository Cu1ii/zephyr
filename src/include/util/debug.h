//
// Created by Cu1 on 2022/9/5.
//

#ifndef ZEPHYR_DEBUG_H
#define ZEPHYR_DEBUG_H

#include <iostream>
#include <cstdio>

namespace zephyr
{

#ifdef (__clang__)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __FUNCSIG__
#else
#error "This compiler is currently not supported by zephyr_debug."
#endif


} // namespace zephyr


#endif //ZEPHYR_DEBUG_H
