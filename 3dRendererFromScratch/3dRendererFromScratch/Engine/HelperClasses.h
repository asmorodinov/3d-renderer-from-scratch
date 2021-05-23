#pragma once

#include <vector>

#include "Pool.h"

namespace eng {

template <typename T>
class Vector2d {
 public:
    Vector2d(size_t width = 0, size_t height = 0, const T& elem = T())
        : width(width), height(height), vec(width * height, elem, Pool::getPool()) {}

    void set(size_t x, size_t y, const T& elem) {
        assert(x < width && y < height);
        vec[y * width + x] = elem;
    }
    const T& get(size_t x, size_t y) const {
        assert(x < width && y < height);
        return vec[y * width + x];
    }
    void fill(const T& elem) { std::fill(vec.begin(), vec.end(), elem); }

 private:
    std::pmr::vector<T> vec;
    size_t width, height;
};

struct EmptyStruct {};
struct NoVariables {
    void operator*=(float t) {}
    NoVariables operator*(float t) const { return {}; }
    NoVariables operator+(const NoVariables& oth) const { return {}; }
};
template <typename T>
struct OneVariable {
    void operator*=(float t_) { t *= t_; }
    OneVariable operator*(float t_) const { return {t * t_}; }
    OneVariable operator+(const OneVariable& oth) const { return {t + oth.t}; }

    T t;
};
template <typename T1, typename T2>
struct TwoVariables {
    void operator*=(float t) {
        t1 *= t;
        t2 *= t;
    }
    TwoVariables operator*(float t) const { return {t1 * t, t2 * t}; }
    TwoVariables operator+(const TwoVariables& oth) const { return {t1 + oth.t1, t2 + oth.t2}; }
    T1 t1;
    T2 t2;
};

}  // namespace eng
