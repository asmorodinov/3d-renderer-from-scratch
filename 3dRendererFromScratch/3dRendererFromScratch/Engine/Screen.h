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

namespace eng {

using Byte = unsigned char;

class Screen {
 public:
    using Pixels = std::size_t;
    using Color = glm::vec3;
    using Depth = float;

    // one byte per color component (alpha included)
    using CompressedColor = glm::u8vec4;

    using AspectRatio = float;
    using Degrees = float;

    // stores 12 bytes per pixel
    using ColorBuffer = Vector2d<Color>;

    // only store 4 bytes per pixel
    using CompressedColorBuffer = Vector2d<CompressedColor>;

    using DepthBuffer = Vector2d<Depth>;

 private:
    static constexpr const Depth defaultNearPlaneDistance_ = 0.2f;
    static constexpr const Depth defaultFarPlaneDistance_ = 10.0f;
    static constexpr const Degrees defaultFieldOfViewAngle_ = 60.0f;

 public:
    Screen(Pixels width, Pixels height, Color clearColor);

    const glm::mat4& getProjectionMatrix() const;

    Color getPixelColor(Pixels x, Pixels y) const;

    const CompressedColorBuffer& getColorBuffer() const;

    void setPixelColor(Pixels x, Pixels y, Color color, Depth z);

    Depth getPixelDepth(Pixels x, Pixels y) const;
    void clear();

    Pixels getWidth() const;
    Pixels getHeight() const;

    Depth getNearPlaneDistance() const;
    void setWriteToDepthBuffer(bool b);

    void setClearColor(Color clr);
    Color getClearColor() const;

 private:
    void setPixelColor(Pixels x, Pixels y, Color color);
    void depthCheckSetPixelColor(Pixels x, Pixels y, Depth z, Color color);
    void setPixelDepth(Pixels x, Pixels y, Depth z);

    Depth nearPlaneDistance_, farPlaneDistance_;
    bool writeToDepthBiffer_ = true;

    Pixels screenWidth_, screenHeight_;
    AspectRatio aspectRatio_ = 1.0f;

    Color clearColor_;

    // color and depth buffers
    CompressedColorBuffer colorBuffer_;
    DepthBuffer depthBuffer_;

    glm::mat4 projectionMatrix_;
};

Screen::CompressedColor compress_color(Screen::Color color);

Screen::Color decompress_color(Screen::CompressedColor color);

}  // namespace eng
