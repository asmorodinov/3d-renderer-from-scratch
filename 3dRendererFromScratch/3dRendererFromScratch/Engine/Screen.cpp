#include "Screen.h"

namespace eng {

Screen::Screen(size_t width, size_t height, ColorType clearColor)
    : near(0.2f),
      far(10.0f),
      width(width),
      height(height),
      ratio(width / (float)height),
      clearColor(clearColor),
      colorBuffer(width, height, clearColor),
      depthBuffer(width, height, std::numeric_limits<float>::max()),
      projectionMatrix(glm::perspective(glm::radians(60.0f), ratio, near, far)) {}

const glm::mat4& Screen::getProjectionMatrix() const { return projectionMatrix; }

ColorType Screen::getPixelColor(size_t x, size_t y) const {
    assert(x < width && y < height);

    return colorBuffer.get(x, y);
}

void Screen::setPixelColor(size_t x, size_t y, ColorType color) {
    assert(x < width && y < height);

    colorBuffer.set(x, y, color);
}

void Screen::setPixelColor(size_t x, size_t y, ColorType color, float z) { depthCheckSetPixelColor(x, y, z, color); }

void Screen::depthCheckSetPixelColor(size_t x, size_t y, float z, ColorType color) {
    assert(x < width && y < height);

    if (z < -1 || z > 1) return;

    if (z < getPixelDepth(x, y)) {
        if (writeToDepthBiffer) setPixelDepth(x, y, z);

        setPixelColor(x, y, color);
    }
}

float Screen::getPixelDepth(size_t x, size_t y) const {
    assert(x < width && y < height);
    return depthBuffer.get(x, y);
}
void Screen::setPixelDepth(size_t x, size_t y, float z) {
    assert(x < width && y < height);

    depthBuffer.set(x, y, z);
}

void Screen::clear() {
    colorBuffer.fill(clearColor);
    depthBuffer.fill(std::numeric_limits<float>::max());
}

size_t Screen::getWidth() const { return width; }
size_t Screen::getHeight() const { return height; }

float Screen::getNearPlaneDistance() const { return near; }

void Screen::setWriteToDepthBuffer(bool b) { writeToDepthBiffer = b; }

void Screen::setClearColor(ColorType clr) { clearColor = clr; }
ColorType Screen::getClearColor() const { return clearColor; }

}  // namespace eng
