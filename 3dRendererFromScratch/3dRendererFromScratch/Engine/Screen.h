#pragma once

#include <cassert>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

using ColorType = glm::vec3;
using ColorBuffer = std::vector<std::vector<ColorType>>;

class Screen {
 public:
    Screen(size_t width, size_t height, ColorType clearColor);

    const glm::mat4& getProjectionMatrix() const;

    ColorType getPixelColor(size_t x, size_t y) const;
    void setPixelColor(size_t x, size_t y, ColorType color);
    void checkAndSetPixelColor(size_t x, size_t y, ColorType color);

    void clear();

    size_t getWidth() const;
    size_t getHeight() const;

 private:
    size_t width, height;
    ColorType clearColor;
    ColorBuffer colorBuffer;

    glm::mat4 projectionMatrix;
};

}  // namespace eng
