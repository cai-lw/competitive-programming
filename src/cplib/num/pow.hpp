#pragma once

namespace cplib {

template<typename R>
constexpr R pow(R base, unsigned long long exp) {
    R res(1);
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