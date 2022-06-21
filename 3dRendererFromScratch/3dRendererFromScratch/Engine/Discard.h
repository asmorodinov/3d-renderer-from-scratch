#pragma once

#include <cstdlib>
#include <cassert>

#include "Types.h"

namespace eng {

// no discard
template <typename Color>
struct NoDiscard {
    static bool discard(Color color) {
        return false;
    }
};

// generic discard
template <typename Color, typename D128, typename D32>
struct GenericDiscard {
    static bool discard(Color color) {
        assert(false);  // do not use this directly
        exit(1);
        return false;
    }
};
template <typename D128, typename D32>
struct GenericDiscard<Color128, D128, D32> {
    static bool discard(Color128 color) {
        return D128::discard(color);
    }
};
template <typename D128, typename D32>
struct GenericDiscard<Color32, D128, D32> {
    static bool discard(Color32 color) {
        return D32::discard(color);
    }
};

// alpha discard
struct AlphaDiscard128 {
    static bool discard(Color128 color) {
        return (color.a == 0.0f);
    }
};

struct AlphaDiscard32 {
    static bool discard(Color32 color) {
        return (color.a == 0);
    }
};

template <typename Color>
using AlphaDiscard = GenericDiscard<Color, AlphaDiscard128, AlphaDiscard32>;

}  // namespace eng
