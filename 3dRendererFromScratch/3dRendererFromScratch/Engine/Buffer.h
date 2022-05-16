#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <cstddef>
#include <cassert>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vector2d.h"

#include "Types.h"

namespace eng {

// helper functions to convert colors
template <typename Color1, typename Color2>
inline Color2 convertColor(Color1 color) {
    assert(false);  // do not use this specific conversion
    return Color2(color);
}

template <typename Color>
inline Color convertColor(Color color) {
    return color;
}
template <>
inline Color32 convertColor(Color128 color) {
    return Color32{Byte(255.99f * color.r), Byte(255.99 * color.g), Byte(255.99 * color.b), Byte(255.99 * color.a)};
}
template <>
inline Color128 convertColor(Color32 color) {
    return Color128{color.r, color.g, color.b, color.a} / 255.0f;
}

template <typename ColorType>
class ColorAndDepthBuffer {
 public:
    using Color = ColorType;
    using ColorBuffer = Vector2d<Color>;
    using DepthBuffer = Vector2d<Depth>;

 public:
    ColorAndDepthBuffer(Pixels width, Pixels height, Color clearColor)
        : width_(width),
          height_(height),
          clearColor_(clearColor),
          colorBuffer_(width, height, clearColor),
          depthBuffer_(width, height, std::numeric_limits<float>::max()) {
    }
    Color getPixelColor(Pixels x, Pixels y) const {
        return colorBuffer_.get(x, y);
    }
    const ColorBuffer& getColorBuffer() const {
        return colorBuffer_;
    }
    void setPixelColor(Pixels x, Pixels y, Color color, Depth z) {
        depthCheckSetPixelColor(x, y, z, color);
    }
    Depth getPixelDepth(Pixels x, Pixels y) const {
        return depthBuffer_.get(x, y);
    }
    void clear() {
        colorBuffer_.fill(clearColor_);
        depthBuffer_.fill(std::numeric_limits<float>::max());
    }
    Pixels getWidth() const {
        return width_;
    }
    Pixels getHeight() const {
        return height_;
    }
    void setWriteToDepthBuffer(bool b) {
        writeToDepthBuffer_ = b;
    }
    void setClearColor(Color clr) {
        clearColor_ = clr;
    }
    Color getClearColor() const {
        return clearColor_;
    }

 private:
    void setPixelColor(Pixels x, Pixels y, Color color) {
        colorBuffer_.set(x, y, color);
    }
    void depthCheckSetPixelColor(Pixels x, Pixels y, Depth z, Color color) {
        if (z < -1 || z > 1) return;
        if (z < getPixelDepth(x, y)) {
            if (writeToDepthBuffer_) setPixelDepth(x, y, z);
            setPixelColor(x, y, color);
        }
    }
    void setPixelDepth(Pixels x, Pixels y, Depth z) {
        depthBuffer_.set(x, y, z);
    }

 private:
    bool writeToDepthBuffer_ = true;
    Pixels width_, height_;
    Color clearColor_;
    // color and depth buffers
    ColorBuffer colorBuffer_;
    DepthBuffer depthBuffer_;
};

}  // namespace eng
