//
// Created by Cu1 on 2022/8/5.
//

#ifndef ZEPHYR_MATH_H
#define ZEPHYR_MATH_H

#include "internal_bit.hpp"

namespace zephyr
{

/**
 *
 * @param p
 * @param n `n >= 0`
 * @return `p ** n`
 */
double pow(double p, int n) {
    double result = 1.0;
    while (n) {
        if (n & 1)
            result = result * p;
        n = (n >> 1);
        p = p * p;
    }
    return result;
}


long long pow_by_mod(long long p,
                     unsigned long long n,
                     unsigned long long mod) {
    long long result = 1;
    while (n) {
        if (n & 1)
            result = (result * p) % mod;
        n = (n >> 1);
        p = (p * p) % mod;
    }
    return result % mod;

}


}



#endif //ZEPHYR_MATH_H
