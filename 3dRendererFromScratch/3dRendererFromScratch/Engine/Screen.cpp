#include "Screen.h"

namespace eng {

Screen::Screen(Pixels width, Pixels height, Color clearColor)
    : nearPlaneDistance_(defaultNearPlaneDistance_),
      farPlaneDistance_(defaultFarPlaneDistance_),
      screenWidth_(width),
      screenHeight_(height),
      aspectRatio_(width / (float)height),
      clearColor_(clearColor),
      colorBuffer_(width, height, compress_color(clearColor)),
      depthBuffer_(width, height, std::numeric_limits<float>::max()),
      projectionMatrix_(glm::perspective(glm::radians(defaultFieldOfViewAngle_), aspectRatio_, nearPlaneDistance_, farPlaneDistance_)) {
}

const glm::mat4& Screen::getProjectionMatrix() const {
    return projectionMatrix_;
}

Screen::Color Screen::getPixelColor(Pixels x, Pixels y) const {
    assert(x < screenWidth_ && y < screenHeight_);

    return decompress_color(colorBuffer_.get(x, y));
}

const Screen::CompressedColorBuffer& Screen::getColorBuffer() const {
    return colorBuffer_;
}

void Screen::setPixelColor(Pixels x, Pixels y, Color color, Depth z) {
    depthCheckSetPixelColor(x, y, z, color);
}

Screen::Depth Screen::getPixelDepth(Pixels x, Pixels y) const {
    assert(x < screenWidth_ && y < screenHeight_);
    return depthBuffer_.get(x, y);
}

void Screen::clear() {
    colorBuffer_.fill(compress_color(clearColor_));
    depthBuffer_.fill(std::numeric_limits<float>::max());
}

Screen::Pixels Screen::getWidth() const {
    return screenWidth_;
}

Screen::Pixels Screen::getHeight() const {
    return screenHeight_;
}

Screen::Depth Screen::getNearPlaneDistance() const {
    return nearPlaneDistance_;
}

void Screen::setWriteToDepthBuffer(bool b) {
    writeToDepthBiffer_ = b;
}

void Screen::setClearColor(Color clr) {
    clearColor_ = clr;
}

Screen::Color Screen::getClearColor() const {
    return clearColor_;
}

void Screen::setPixelColor(Pixels x, Pixels y, Color color) {
    assert(x < screenWidth_ && y < screenHeight_);

    colorBuffer_.set(x, y, compress_color(color));
}

void Screen::depthCheckSetPixelColor(Pixels x, Pixels y, Depth z, Color color) {
    assert(x < screenWidth_ && y < screenHeight_);

    if (z < -1 || z > 1) return;

    if (z < getPixelDepth(x, y)) {
        if (writeToDepthBiffer_) setPixelDepth(x, y, z);

        setPixelColor(x, y, color);
    }
}

void Screen::setPixelDepth(Pixels x, Pixels y, Depth z) {
    assert(x < screenWidth_ && y < screenHeight_);

    depthBuffer_.set(x, y, z);
}

Screen::CompressedColor compress_color(Screen::Color color) {
    return Screen::CompressedColor(Byte(255.99f * color.r), Byte(255.99 * color.g), Byte(255.99 * color.b), Byte(255));
}

Screen::Color decompress_color(Screen::CompressedColor color) {
    return Screen::Color(color.r, color.g, color.b) / 255.0f;
}

}  // namespace eng
