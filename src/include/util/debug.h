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
#include <ios>
#include <iomanip>
#include <chrono>
#include <cstdio>
#include <vector>
#include <type_traits>
#include <memory>
#include <ctime>
#include <tuple>
#include <sstream>


#ifdef ZEPHYR_DEBUG_UNIX
#include <unistd.h>
#endif

struct time {};

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

struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(const nonesuch&) = delete;
    void operator=(const nonesuch&) = delete;
};

template <typename...>
using void_t = void;

template <typename Default,
          typename AlwatsVoid,
          template<typename... name>
          typename Op,
          typename... Args>
struct detector {
    using value_t = std::false_type;
    using type = Default;
};

template <typename Default,
          template <typename...>
          typename Op,
          typename... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
    using value_t =std::true_type;
    using type = Op<Args...>;
};

template <template <typename...>
          typename Op,
          typename... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template <typename T>
constexpr auto size(const T& c) -> decltype(c.size()) {
    return c.size();
}

template <typename T, size_t N>
constexpr size_t size(const T (&)[N]) {
    return N;
}

template <typename Container>
using container_begin_t = decltype(std::begin(std::declval<Container>()));

template <typename Container>
using container_end_t = decltype(std::end(std::declval<Container>()));

template <typename Container>
using container_size_t = decltype(size(std::declval<Container>()));


template <typename T>
struct is_container {
    static constexpr bool value =
            is_detected<container_begin_t, T>::value &&
            is_detected<container_end_t, T>::value &&
            is_detected<container_size_t, T>::value &&
            !std::is_same<std::string,
                          typename std::remove_cv<
                                  typename std::remove_reference<T>::type>::type>::value;
};

template <typename T>
using ostream_operator_t =
    decltype(std::declval<std::ostream&>() << std::declval<T>());

template <typename T>
struct has_ostream_operator
        : is_detected<ostream_operator_t, T>
{};

template <typename T>
struct print_type {};

template <typename T>
inline void pretty_print(std::ostream& stream, const T& value, std::true_type);

template <typename T>
inline void pretty_print(std::ostream& stream, const T& value, std::false_type);

template <typename T>
inline typename std::enable_if<!is_container<const T&>::value &&
                                !std::is_enum<T>::value, bool>::type
pretty_print(std::ostream& stream, const T& value);

inline bool pretty_print(std::ostream& stream,const bool value);

inline bool pretty_print(std::ostream& stream,const char value);

template <typename T>
inline bool pretty_print(std::ostream& stream, const T* const value);

template <typename T, typename Deleter>
inline bool pretty_print(std::ostream& stream,
                         std::unique_ptr<T, Deleter>& value);

template <typename T>
inline bool pretty_print(std::ostream& stream, std::shared_ptr<T>& value);

template <size_t N>
inline bool pretty_print(std::ostream& stream, const char (&value)[N]);

template <>
inline bool pretty_print(std::ostream& stream, const char* const value);

template <typename... T>
inline bool pretty_print(std::ostream& stream, const std::tuple<T...>& value);

template <>
inline bool pretty_print(std::ostream& stream, const std::tuple<>&);

template <>
inline bool pretty_print(std::ostream& stream, const struct time&);


template <typename T>
inline bool pretty_print(std::ostream& stream,
                         const integral_print_formatter<T>& value);

template <typename T>
inline bool pretty_print(std::ostream& stream, const print_type<T>&);

template <typename Enum>
inline typename std::enable_if<std::is_enum<Enum>::value, bool>::type
        pretty_print(std::ostream& stream, const Enum& value);

inline bool pretty_print(std::ostream& stream, const std::string& value);

template <typename T1, typename T2>
inline bool pretty_print(std::ostream& stream, const std::pair<T1, T2>& value);


template <typename Container>
inline typename std::enable_if<is_container<const Container&>::value, bool>::type
pretty_print(std::ostream& stream, const Container& value);

template <typename T>
inline void pretty_print(std::ostream& stream, const T& value, std::true_type) {
    stream << value;
}

template <typename T>
inline void pretty_print(std::ostream& stream, const T& value, std::false_type) {
    static_assert(has_ostream_operator<const T&>::value,
                  "Type does not support the << ostream operator");
}

template <typename T>
inline typename std::enable_if<!is_container<const T&>::value &&
                                !std::is_enum<T>::value, bool>::type
pretty_print(std::ostream& stream, const T& value) {
    pretty_print(stream, value, typename has_ostream_operator<const T&>::type());
    return true;
}

inline bool pretty_print(std::ostream& stream,const bool value) {
    stream << std::boolalpha << value;
    return true;
}

inline bool pretty_print(std::ostream& stream,const char value) {
    const bool printable = (value >= 0x20 && value <= 0x7E);
    if (printable) {
        stream << "'" << value << "'";
    }
    else {
        stream << "'\\x" << std::setw(2) << std::setfill('0') << std::hex
            << std::uppercase << (0xFF & value) << "'";
    }
}

template <typename T>
inline bool pretty_print(std::ostream& stream, const T* const value) {
    if (value == nullptr) {
        stream << "nullptr";
    }
    else {
        stream << value;
    }
    return true;
}

template <typename T, typename Deleter>
inline bool pretty_print(std::ostream& stream,
                         std::unique_ptr<T, Deleter>& value) {
    pretty_print(stream, value.get());
    return true;
}

template <typename T>
inline bool pretty_print(std::ostream& stream, std::shared_ptr<T>& value) {
    pretty_print(stream, value.get());
    stream << " (use_count = " << value.use_count() << ")";
    return true;
}

template <size_t N>
inline bool pretty_print(std::ostream& stream, const char (&value)[N]) {
    stream << value;
    return false;
}

template <>
inline bool pretty_print(std::ostream& stream, const char* const value) {
    stream << '"' << value << '"';
}

template <size_t Idx>
struct pretty_print_tuple {
    template <typename... Ts>
    static void print(std::ostream& stream, const std::tuple<Ts...>& tuple) {
        pretty_print_tuple<Idx - 1>::print(stream, tuple);
        stream << ", ";
        pretty_print(stream, std::get<Idx>(tuple));
    }
};

template <>
struct pretty_print_tuple<0> {
    template <typename... Ts>
    static void print(std::ostream& stream, const std::tuple<Ts...>& tuple) {
        pretty_print(stream, std::get<0>(tuple));
    }
};

template <typename... T>
inline bool pretty_print(std::ostream& stream, const std::tuple<T...>& value) {
    stream << "{";
    pretty_print_tuple<sizeof...(T) - 1>::print(stream, value);
    stream << "}";
}

template <>
inline bool pretty_print(std::ostream& stream, const std::tuple<>&) {
    stream << "{ }";
}

template <>
inline bool pretty_print(std::ostream& stream, const struct time&) {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto us =
            duration_cast<microseconds>(now.time_since_epoch()).count() % 1000000;
    const auto hms = system_clock::to_time_t(now);
#if _MSC_VER >= 1600
    struct tm t;
    localtime_s(&t, &hms);
    const std::tm* tm = &t;
#else
    const std::tm* tm = std::localtime(&hms);
#endif
    stream << "current time = " << std::put_time(tm, "%H:%M:%S") << '.'
           << std::setw(6) << std::setfill('0') << us;
    return false;
}


template <typename T>
inline bool pretty_print(std::ostream& stream,
                         const integral_print_formatter<T>& value) {

}

template <typename T>
inline bool pretty_print(std::ostream& stream, const print_type<T>&) {

}

template <typename Enum>
inline typename std::enable_if<std::is_enum<Enum>::value, bool>::type
        pretty_print(std::ostream& stream, const Enum& value) {

}

inline bool pretty_print(std::ostream& stream, const std::string& value) {

}

template <typename T1, typename T2>
inline bool pretty_print(std::ostream& stream, const std::pair<T1, T2>& value) {

}


template <typename Container>
inline typename std::enable_if<is_container<const Container&>::value, bool>::type
pretty_print(std::ostream& stream, const Container& value) {

}



} // namespace zephyr


#endif //ZEPHYR_DEBUG_H
