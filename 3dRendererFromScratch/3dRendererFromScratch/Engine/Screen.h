#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <cassert>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "HelperClasses.h"

namespace eng {

using ColorType = glm::vec3;
using ColorBuffer = Vector2d<ColorType>;
using DepthBuffer = Vector2d<float>;

class Screen {
 public:
    Screen(size_t width, size_t height, ColorType clearColor);

    const glm::mat4& getProjectionMatrix() const;

    ColorType getPixelColor(size_t x, size_t y) const;
    void setPixelColor(size_t x, size_t y, ColorType color, float z);

    float getPixelDepth(size_t x, size_t y) const;
    void clear();

    size_t getWidth() const;
    size_t getHeight() const;

    float getNearPlaneDistance() const;
    void setWriteToDepthBuffer(bool b);

    void setClearColor(ColorType clr);
    ColorType getClearColor() const;

 private:
    void setPixelColor(size_t x, size_t y, ColorType color);
    void depthCheckSetPixelColor(size_t x, size_t y, float z, ColorType color);
    void setPixelDepth(size_t x, size_t y, float z);

    float near, far;
    bool writeToDepthBiffer = true;

    size_t width, height;
    float ratio = 1.0f;
    ColorType clearColor;
    ColorBuffer colorBuffer;
    DepthBuffer depthBuffer;

    glm::mat4 projectionMatrix;
};

}  // namespace eng
