#pragma once

#include <algorithm>
#include <iostream>
#include <limits>
#include <cassert>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

using ColorType = glm::vec3;
using ColorBuffer = std::vector<std::vector<ColorType>>;
using DepthBuffer = std::vector<std::vector<float>>;

class Screen {
 public:
    Screen(size_t width, size_t height, ColorType clearColor);

    const glm::mat4& getProjectionMatrix() const;

    ColorType getPixelColor(size_t x, size_t y) const;
    void setPixelColor(size_t x, size_t y, ColorType color);
    void setPixelColor(size_t x, size_t y, ColorType color, float z);

    void checkAndSetPixelColor(size_t x, size_t y, ColorType color);

    void depthCheckSetPixelColor(size_t x, size_t y, float z, ColorType color);

    float getPixelDepth(size_t x, size_t y) const;
    void setPixelDepth(size_t x, size_t y, float z);

    void clear();

    glm::vec4 localSpaceToScreenSpace(const glm::mat4& viewMatrix, const glm::mat4& objectModel, glm::vec3 localCoords) const;

    size_t getWidth() const;
    size_t getHeight() const;

    void setClearColor(ColorType clr);
    ColorType getClearColor() const;

 public:
    float near, far;

 private:
    size_t width, height;
    float ratio = 1.0f;
    ColorType clearColor;
    ColorBuffer colorBuffer;
    DepthBuffer depthBuffer;

    glm::mat4 projectionMatrix;

    // float minz = 1.2f;
    // float maxz = -1.2f;
};

}  // namespace eng
