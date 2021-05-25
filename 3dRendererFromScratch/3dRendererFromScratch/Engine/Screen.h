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

class Screen {
 public:
    using Pixels = std::size_t;
    using Color = glm::vec3;
    using Depth = float;
    using AspectRatio = float;

    using ColorBuffer = Vector2d<Color>;
    using DepthBuffer = Vector2d<Depth>;

    Screen(Pixels width, Pixels height, Color clearColor);

    const glm::mat4& getProjectionMatrix() const;

    Color getPixelColor(Pixels x, Pixels y) const;
    void setPixelColor(Pixels x, Pixels y, Color color, Depth z);

    float getPixelDepth(Pixels x, Pixels y) const;
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
    ColorBuffer colorBuffer_;
    DepthBuffer depthBuffer_;

    glm::mat4 projectionMatrix_;
};

}  // namespace eng
