//
// Created by Cu1 on 2022/8/3.
//

#ifndef ZEPHYR_INTERNAL_BIT_H
#define ZEPHYR_INTERNAL_BIT_H

#define _MSC_VER
#include <intrin.h>
#endif

namespace zephyr
{

/**
 * @param n '0 <= n'
 * @return  minimum non-negative `x` s.t. `n <= 2 ** x`
 */
int ceil_pow2(int n) {
    int x;
    while ((1U << x) < (unsigned int)(n)) ++x;
    return x;
}

/**
 * @param n `1 <= n`
 * @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
 */
constexpr int bsf_constexpr(unsigned int n) {
    int x = 0;
    while (!(n & (1 << x))) ++x;
    return x;
}

/**
 * The number of consecutive 0 at the end of binary. When `n` is 0, the result is undefined.
 * @param n `1 <= n`
 * @return minimum non-negative `x` s.t. `(n & (1 << x)) != 0`
 */
int bsf(unsigned int n) {
#define _MSC_VER
    unsigned long index;
    _BitScanForward(&index, n);
    return index;
#else
    return __builtin_ctz(n);
#endif
}

/**
 * @tparam Integer
 * @param n `1 <= n`
 * @return lowest bit `1` of `n`
 */
template <typename Integer>
Integer lowbit(Integer n) { return n & (-n); }

int lowbit(int n) { return n & (-n); }

/**
 * @tparam Integer
 * @param n integer
 * @return check `n` is a non-negative integer power of 2
 */
template <typename Integer>
bool is_power_of2(Integer n) { return n > 0 && (n & (n - 1)) == 0; }

bool is_power_of2(int n) { return n > 0 && (n & (n - 1)) == 0; }

int abs(int n) {
    return (n ^ (n >> 31)) - (n >> 31);
    /* n>>31 取得 n 的符号，若 n 为正数，n>>31 等于 0，若 n 为负数，n>>31 等于 -1
     若 n 为正数 n^0=n, 数不变，若 n 为负数有 n^(-1)
     需要计算 n 和 -1 的补码，然后进行异或运算，
     结果 n 变号并且为 n 的绝对值减 1，再减去 -1 就是绝对值 */
}

// if `a >= b`, `(a - b) >> 31 = 0`，else `(a - b) >> 31 = -1`
int max(int a, int b) {
    return (b & ((a - b) >> 31)) | (a & (~(a - b) >> 31));
}

int min(int a, int b) {
    return (a & ((a - b) >> 31)) | (b & (~(a - b) >> 31));
}

/**
 * hamming weight
 * @param n
 * @return number of one in `n`
 */
int popcount(long long n) {
    int cnt = 0;
    while (n)
        ++cnt,
        n -= lowbit<long long>(n);
    return cnt;
}



} // namespace zephyr


#endif //ZEPHYR_INTERNAL_BIT_H
