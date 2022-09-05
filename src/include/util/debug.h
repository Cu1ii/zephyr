//
// Created by Cu1 on 2022/9/5.
//

#ifndef ZEPHYR_DEBUG_H
#define ZEPHYR_DEBUG_H

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#define ZEPHYR_DEBUG_UNIX
#elif defined(_MSC_VER)
#define ZEPHYR_DEBUG_WINDOWS
#endif

#include <iostream>
#include <cstdio>

#ifdef ZEPHYR_DEBUG_UNIX
#include <unistd.h>
#endif

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
