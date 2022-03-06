#pragma once

namespace cplib {

template<typename T>
constexpr T pow(T base, unsigned long long exp) {
    T res(1);
    while (exp) {
        if (exp & 1) {
            res *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return res;
}

}  // namespace cplib