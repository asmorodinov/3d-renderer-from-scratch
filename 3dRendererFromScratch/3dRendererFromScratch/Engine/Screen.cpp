#include "Screen.h"

namespace eng {

Screen::Screen(size_t width, size_t height, ColorType clearColor)
    : near(0.001f),
      far(10.0f),
      width(width),
      height(height),
      ratio(width / (float)height),
      clearColor(clearColor),
      colorBuffer(width, std::vector<ColorType>(height, clearColor)),
      depthBuffer(width, std::vector<float>(height, std::numeric_limits<float>::max())),
      projectionMatrix(glm::perspectiveRH_NO(glm::radians(45.0f), ratio, near, far)) {}

const glm::mat4& Screen::getProjectionMatrix() const { return projectionMatrix; }

ColorType Screen::getPixelColor(size_t x, size_t y) const {
    assert(x < width && y < height);

    return colorBuffer[x][y];
}

void Screen::setPixelColor(size_t x, size_t y, ColorType color) {
    assert(x < width && y < height);

    colorBuffer[x][y] = color;
}

void Screen::setPixelColor(size_t x, size_t y, ColorType color, float z) {
    // setPixelColor(x, y, color);
    // return;
    depthCheckSetPixelColor(x, y, z, color);
}

void Screen::checkAndSetPixelColor(size_t x, size_t y, ColorType color) {
    if (x < width || y < height) return;

    colorBuffer[x][y] = color;
}

void Screen::depthCheckSetPixelColor(size_t x, size_t y, float z, ColorType color) {
    assert(x < width && y < height);

    if (z < -1 || z > 1) return;

    if (z < getPixelDepth(x, y)) {
        if (setDepth) setPixelDepth(x, y, z);

        setPixelColor(x, y, color);
    }
}

float Screen::getPixelDepth(size_t x, size_t y) const {
    assert(x < width && y < height);
    return depthBuffer[x][y];
}
void Screen::setPixelDepth(size_t x, size_t y, float z) {
    assert(x < width && y < height);

    depthBuffer[x][y] = z;
}

void Screen::clear() {
    colorBuffer = ColorBuffer(width, std::vector<ColorType>(height, clearColor));
    depthBuffer = DepthBuffer(width, std::vector<float>(height, std::numeric_limits<float>::max()));

    // std::cout << "min: " << minz << " max: " << maxz << '\n';
}

glm::vec4 Screen::localSpaceToScreenSpace(const glm::mat4& viewMatrix, const glm::mat4& objectModel,
                                          glm::vec3 localCoords) const {
    glm::vec4 worldCoords = objectModel * glm::vec4(localCoords, 1.0f);

    glm::vec4 position = projectionMatrix * viewMatrix * worldCoords;

    return glm::vec4(position.x / position.w, position.y / position.w, position.z / position.w, position.w);
}

size_t Screen::getWidth() const { return width; }
size_t Screen::getHeight() const { return height; }

void Screen::setClearColor(ColorType clr) { clearColor = clr; }
ColorType Screen::getClearColor() const { return clearColor; }

}  // namespace eng
