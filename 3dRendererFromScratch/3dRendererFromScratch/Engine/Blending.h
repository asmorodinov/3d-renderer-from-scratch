#pragma once

#include <cstdlib>
#include <cassert>

#include "Conversion.h"
#include "Types.h"

namespace eng {

// no blending
template <typename Color>
struct NoBlending {
    static Color blend(Color source, Color destination) {
        return source;
    }
};

// generic blending
template <typename Color, typename B128, typename B32>
struct GenericBlending {
    static Color blend(Color source, Color destination) {
        assert(false);  // do not use this directly
        exit(1);
        return source;
    }
};
template <typename B128, typename B32>
struct GenericBlending<Color128, B128, B32> {
    static Color128 blend(Color128 source, Color128 destination) {
        return B128::blend(source, destination);
    }
};
template <typename B128, typename B32>
struct GenericBlending<Color32, B128, B32> {
    static Color32 blend(Color32 source, Color32 destination) {
        return B32::blend(source, destination);
    }
};

// alpha blending
struct AlphaBlending128 {
    static Color128 blend(Color128 source, Color128 destination) {
        return source * source.a + destination * (1 - source.a);
    }
};

struct AlphaBlending32 {
    // this is probably quite a slow way of doing this blending
    static Color32 blend(Color32 source, Color32 destination) {
        return ClampConversion<Color128, Color32>::convertColor(AlphaBlending128::blend(DefaultConversion<Color32, Color128>::convertColor(source),
                                                                                        DefaultConversion<Color32, Color128>::convertColor(destination)));
    }
};

template <typename Color>
using AlphaBlending = GenericBlending<Color, AlphaBlending128, AlphaBlending32>;

}  // namespace eng
