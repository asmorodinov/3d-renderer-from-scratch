#pragma once

#include <tuple>
#include <utility>
#include <vector>

namespace eng {

struct EmptyStruct {};
struct NoVariables {
    void operator*=(float t) {
    }
    NoVariables operator*(float t) const {
        return {};
    }
    NoVariables operator+(const NoVariables& oth) const {
        return {};
    }
};
template <typename T>
struct OneVariable {
    void operator*=(float t_) {
        t *= t_;
    }
    OneVariable operator*(float t_) const {
        return {t * t_};
    }
    OneVariable operator+(const OneVariable& oth) const {
        return {t + oth.t};
    }

    T t;
};
template <typename T1, typename T2>
struct TwoVariables {
    void operator*=(float t) {
        t1 *= t;
        t2 *= t;
    }
    TwoVariables operator*(float t) const {
        return {t1 * t, t2 * t};
    }
    TwoVariables operator+(const TwoVariables& oth) const {
        return {t1 + oth.t1, t2 + oth.t2};
    }
    T1 t1;
    T2 t2;
};
template <typename T1, typename T2, typename T3>
struct ThreeVariables {
    void operator*=(float t) {
        t1 *= t;
        t2 *= t;
        t3 *= t;
    }
    ThreeVariables operator*(float t) const {
        return {t1 * t, t2 * t, t3 * t};
    }
    ThreeVariables operator+(const ThreeVariables& oth) const {
        return {t1 + oth.t1, t2 + oth.t2, t3 + oth.t3};
    }
    T1 t1;
    T2 t2;
    T3 t3;
};

}  // namespace eng
