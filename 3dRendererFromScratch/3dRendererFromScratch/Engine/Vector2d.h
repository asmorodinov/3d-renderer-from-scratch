#pragma once

#include <cstddef>
#include <vector>
#include "Pool.h"

namespace eng {

template <typename T>
class Vector2d {
 public:
    Vector2d(size_t width = 0, size_t height = 0, const T& elem = T())
        : matrixWidth_(width), matrixHeight_(height), vector_(width * height, elem, Pool::getPool()) {
    }

    void set(size_t x, size_t y, const T& elem) {
        assert(x < matrixWidth_ && y < matrixHeight_);
        vector_[y * matrixWidth_ + x] = elem;
    }
    const T& get(size_t x, size_t y) const {
        assert(x < matrixWidth_ && y < matrixHeight_);
        return vector_[y * matrixWidth_ + x];
    }
    void fill(const T& elem) {
        std::fill(vector_.begin(), vector_.end(), elem);
    }

    const T* get_pointer() const {
        return &vector_[0];
    }

 private:
    std::pmr::vector<T> vector_;
    size_t matrixWidth_, matrixHeight_;
};

}  // namespace eng
