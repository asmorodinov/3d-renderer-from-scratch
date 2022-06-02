#pragma once

#include <cassert>
#include <cstdlib>

#include "Types.h"

namespace eng {

// conversion with some transformations
// T1: Color128 -> Color128
// T2: Color32 -> Color32

template <typename Color1, typename Color2, typename T1, typename T2>
struct TransformationConversion {
    static Color2 convertColor(Color1 color) {
        assert(false);  // do not use this specific conversion
        exit(1);
        return Color2();
    }
};
template <typename Color, typename T1, typename T2>
struct TransformationConversion<Color, Color, T1, T2> {
    static Color convertColor(Color color) {
        return color;
    }
};
template <typename T1, typename T2>
struct TransformationConversion<Color128, Color32, T1, T2> {
    static Color32 convertColor(Color128 color) {
        color = T1::transform(color);
        return Color32{Byte(255.99f * color.r), Byte(255.99 * color.g), Byte(255.99 * color.b), Byte(255.99 * color.a)};
    }
};
template <typename T1, typename T2>
struct TransformationConversion<Color32, Color128, T1, T2> {
    static Color128 convertColor(Color32 color) {
        color = T2::transform(color);
        return Color128{color.r, color.g, color.b, color.a} / 255.0f;
    }
};

// default conversion

struct IdentityT1 {
    static Color128 transform(Color128 color) {
        return color;
    }
};
struct IdentityT2 {
    static Color32 transform(Color32 color) {
        return color;
    }
};
template <typename Color1, typename Color2>
using DefaultConversion = TransformationConversion<Color1, Color2, IdentityT1, IdentityT2>;

// clamp conversion

struct ClampT1 {
    static Color128 transform(Color128 color) {
        return glm::clamp(color, 0.0f, 1.0f);
    }
};

template <typename Color1, typename Color2>
using ClampConversion = TransformationConversion<Color1, Color2, ClampT1, IdentityT2>;

// tone mapping (hdr) + gamma correction

struct HDRT1 {
    static Color128 transform(Color128 color) {
        auto exposure = 1.0f;
        auto gamma = 2.2f;

        auto hdr = glm::vec3(color.r, color.g, color.b);
        // tone mapping
        auto mapped = 1.0f - glm::exp(-hdr * exposure);
        // gamma correction
        mapped = glm::pow(mapped, glm::vec3(1.0f / gamma));

        return Color128(mapped, color.a);
    }
};

template <typename Color1, typename Color2>
using HDRConversion = TransformationConversion<Color1, Color2, HDRT1, IdentityT2>;

}  // namespace eng
