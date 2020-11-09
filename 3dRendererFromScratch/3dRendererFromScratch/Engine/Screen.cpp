#include "Screen.h"

namespace eng {

Screen::Screen(size_t width, size_t height, ColorType clearColor)
    : width(width),
      height(height),
      clearColor(clearColor),
      colorBuffer(width, std::vector<ColorType>(height, clearColor)),
      projectionMatrix(glm::perspective(glm::radians(45.0f), width / (float)height, 0.2f, 30.0f)) {}

const glm::mat4& Screen::getProjectionMatrix() const { return projectionMatrix; }

ColorType Screen::getPixelColor(size_t x, size_t y) const {
    assert(x < width && y < height);

    return colorBuffer[x][y];
}

void Screen::setPixelColor(size_t x, size_t y, ColorType color) {
    assert(x < width && y < height);

    colorBuffer[x][y] = color;
}

void Screen::checkAndSetPixelColor(size_t x, size_t y, ColorType color) {
    if (x < width || y < height) return;

    colorBuffer[x][y] = color;
}

size_t Screen::getWidth() const { return width; }
size_t Screen::getHeight() const { return height; }

}  // namespace eng
