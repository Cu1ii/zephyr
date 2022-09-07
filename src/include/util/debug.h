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
#include <vector>

#ifdef ZEPHYR_DEBUG_UNIX
#include <unistd.h>
#endif

namespace zephyr
{

#if defined(__clang__)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr size_t PREFIX_LENGTH =
        sizeof("const char* zephyr::type_name_impl() [T = ") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof("]") - 1;
#elif defined(__GNUC__) && !defined(__clang__)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __PRETTY_FUNCTION__
static constexpr size_t PREFIX_LENGTH =
        sizeof("const char* zephyr::type_name_impl() [with T = ") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof("]") - 1;
#elif defined(_MSC_VER)
#define ZEPHYR_DEBUG_PRETTY_FUNCTION __FUNCSIG__
static constexpr size_t PREFIX_LENGTH =
    sizeof("const char* __cdecl zephyr::type_name_impl<") - 1;
static constexpr size_t SUFFIX_LENGTH = sizeof(">(void)") - 1;
#else
#error "This compiler is currently not supported by zephyr_debug."
#endif


template <typename T>
struct integral_print_formatter {
public:
    static_assert(std::is_integral<T>::value,
                "Only integral types are supported");

    integral_print_formatter(T value, int numeric_base)
        : integral_formatter_value(value),
          base(numeric_base)
    {}

    operator T() const noexcept { return integral_formatter_value; }

    const char* prefix() const noexcept {
        return base == 2 ? "0b" :
              (base == 8 ? "0o" :
              (base == 16 ? "0x" : ""));
    }

public:

    T integral_formatter_value;
    int base;
};

template <typename T>
integral_print_formatter<T> hex(T value) {
    return integral_print_formatter<T>{value, 16};
}

template <typename T>
integral_print_formatter<T> oct(T value) {
    return integral_print_formatter<T>{value, 8};
}

template <typename T>
integral_print_formatter<T> bin(T value) {
    return integral_print_formatter<T>{value, 2};
}

template <typename T>
const char* type_name_impl() {
    return ZEPHYR_DEBUG_PRETTY_FUNCTION;
}

template <typename T>
struct type_tag {};

template <typename T>
std::string get_type_name(type_tag<T>) {
    std::string type = type_name_impl<T>();
    return type.substr(PREFIX_LENGTH,
                       type.size() - PREFIX_LENGTH - SUFFIX_LENGTH);
}

template <typename T>
std::string type_name() {
    if (std::is_volatile<T>::value) {
        if (std::is_pointer<T>::value) {
            return type_name <typename std::remove_volatile<T>::type>() + " volatile";
        }
        else {
            return "volatile " + type_name <typename std::remove_volatile<T>::type>();
        }
    }
    if (std::is_const<T>::value) {
        if (std::is_pointer<T>::value) {
            return type_name <typename std::remove_const<T>::type>() + " const";
        }
        else {
            return "const " + type_name <typename std::remove_const<T>::type>();
        }
    }
    if (std::is_pointer<T>::value) {
        return type_name <typename std::remove_pointer<T>::type>() + "* ";
    }
    if (std::is_lvalue_reference<T>::value) {
        return type_name <typename std::remove_reference<T>::type>() + "& ";
    }
    if (std::is_rvalue_reference<T>::value) {
        return type_name <typename std::remove_reference<T>::type>() + "&& ";
    }
    return get_type_name(type_tag<T>{});
}

inline std::string get_type_name(type_tag<short>) {
    return "short";
}

inline std::string get_type_name(type_tag<unsigned short>) {
    return "unsigned short";
}

inline std::string get_type_name(type_tag<long>) {
    return "long";
}

inline std::string get_type_name(type_tag<unsigned long>) {
    return "unsigned long";
}

inline std::string get_type_name(type_tag<long long>) {
    return "long long";
}

inline std::string get_type_name(type_tag<unsigned long long>) {
    return "unsigned long long";
}

inline std::string get_type_name(type_tag<std::string>) {
    return "std::string";
}

template <typename T, typename Alloc>
std::string get_type_name(type_tag<std::vector<T, Alloc>>) {
    return "std::vector<" + type_name<T>() + ">";
}

template <typename T1, typename T2>
std::string get_type_name(type_tag<std::pair<T1, T2>>) {
    return "std::pair<" + type_name<T1>() + ", " + type_name<T2>() + ">";
}

template <typename... T>
std::string type_list_to_string() {
    std::string result;

    auto unused = {(result += type_name<T>() + ", ", 0)..., 0};
    static_cast<void>(unused);
    if (sizeof...(T) > 0) {
        result.pop_back();
        result.pop_back();
    }
    return result;

}

template <typename... T>
std::string get_type_name(type_tag<std::tuple<T...>>) {
    return "std::tuple<" + type_list_to_string<T...>() + ">";
}

template <typename T>
inline std::string get_type_name(type_tag<integral_print_formatter<T>>) {
    return type_name<T>();
}





} // namespace zephyr


#endif //ZEPHYR_DEBUG_H
