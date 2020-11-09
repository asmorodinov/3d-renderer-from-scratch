#include "Object.h"

namespace eng {

Transform& Object::getTransform() { return t; }

Point::Point(glm::vec3 position, ColorType color, unsigned pixelRadius) : color(color), pixelRadius(pixelRadius) {
    t.position = position;
}

void Point::draw(const Camera& camera, Screen& screen) {
    glm::vec2 screenCoords =
        LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), glm::vec3(0.0f));

    if (screenCoords.x <= -1.0f || screenCoords.x >= 1.0f || screenCoords.y <= -1.0f || screenCoords.y >= 1.0f) return;

    int x = int(screen.getWidth() * (screenCoords.x + 1.0f) / 2.0f);
    int y = int(screen.getHeight() * (screenCoords.y + 1.0f) / 2.0f);

    for (int i = x - pixelRadius; i <= x + pixelRadius; ++i)
        for (int j = y - pixelRadius; j <= y + pixelRadius; ++j) {
            if (i < 0 || j < 0) continue;

            screen.setPixelColor(size_t(i), size_t(j), color);
        }
}

void Point::update(float dt) {}

}  // namespace eng
