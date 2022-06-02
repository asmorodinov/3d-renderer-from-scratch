#pragma once

#include <limits>
#include <cstddef>

#include "Vector2d.h"

#include "Types.h"
#include "Blending.h"
#include "Conversion.h"
#include "Discard.h"

namespace eng {

template <typename ColorType, template <typename Color1, typename Color2> class ConvertionType, template <typename Color> class BlendingType,
          template <typename Color> class DiscardType>
class ColorAndDepthBuffer {
 public:
    using Color = ColorType;
    using ColorBuffer = Vector2d<Color>;
    using DepthBuffer = Vector2d<Depth>;

    template <typename Color1, typename Color2>
    using Convertion = ConvertionType<Color1, Color2>;
    template <typename Color>
    using Blending = BlendingType<Color>;
    template <typename Color>
    using Discard = DiscardType<Color>;

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
    const DepthBuffer& getDepthBuffer() const {
        return depthBuffer_;
    }

    ColorBuffer& getColorBuffer() {
        return colorBuffer_;
    }
    DepthBuffer& getDepthBuffer() {
        return depthBuffer_;
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

    template <typename Color1, typename Color2>
    static Color2 convertColor(Color1 color) {
        return Convertion<Color1, Color2>::convertColor(color);
    }
    template <typename Clr>
    static Clr blend(Clr source, Clr destination) {
        return Blending<Clr>::blend(source, destination);
    }
    template <typename Clr>
    static bool discard(Clr color) {
        return Discard<Clr>::discard(color);
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
