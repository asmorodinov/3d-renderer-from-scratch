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

Line::Line(glm::vec3 pos1, glm::vec3 pos2, ColorType color) : color(color), pos1(pos1), pos2(pos2) {}

void Line::draw(const Camera& camera, Screen& screen) {
    glm::vec2 p1 = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), pos1);
    glm::vec2 p2 = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), pos2);

    if (p1.x <= -1.0f || p1.x >= 1.0f || p1.y <= -1.0f || p1.y >= 1.0f) return;
    if (p2.x <= -1.0f || p2.x >= 1.0f || p2.y <= -1.0f || p2.y >= 1.0f) return;

    int x0 = int(screen.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y0 = int(screen.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x1 = int(screen.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y1 = int(screen.getHeight() * (p2.y + 1.0f) / 2.0f);

    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy; /* error value e_xy */
    while (true) {     /* loop */
        screen.setPixelColor(size_t(x0), size_t(y0), color);
        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) { /* e_xy+e_x > 0 */
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) { /* e_xy+e_y < 0 */
            err += dx;
            y0 += sy;
        }
    }
}

void Line::update(float dt) {}

Triangle::Triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color)
    : color(color), pos1(pos1), pos2(pos2), pos3(pos3) {}

void Triangle::draw(const Camera& camera, Screen& screen) {
    glm::vec2 p1 = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), pos1);
    glm::vec2 p2 = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), pos2);
    glm::vec2 p3 = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(), t.getModel(), pos3);

    if (p1.x <= -1.0f || p1.x >= 1.0f || p1.y <= -1.0f || p1.y >= 1.0f) return;
    if (p2.x <= -1.0f || p2.x >= 1.0f || p2.y <= -1.0f || p2.y >= 1.0f) return;
    if (p3.x <= -1.0f || p3.x >= 1.0f || p3.y <= -1.0f || p3.y >= 1.0f) return;

    int x0 = int(screen.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y0 = int(screen.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x1 = int(screen.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y1 = int(screen.getHeight() * (p2.y + 1.0f) / 2.0f);

    int x2 = int(screen.getWidth() * (p3.x + 1.0f) / 2.0f);
    int y2 = int(screen.getHeight() * (p3.y + 1.0f) / 2.0f);

    int minx = std::min({x0, x1, x2});
    int miny = std::min({y0, y1, y2});
    int maxx = std::max({x0, x1, x2});
    int maxy = std::max({y0, y1, y2});

    auto e01 = [&](int x, int y) { return (x - x0) * (y1 - y0) - (y - y0) * (x1 - x0); };
    auto e12 = [&](int x, int y) { return (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1); };
    auto e20 = [&](int x, int y) { return (x - x2) * (y0 - y2) - (y - y2) * (x0 - x2); };

    auto insideTriangle = [&](int x, int y) { return e01(x, y) >= 0 && e12(x, y) >= 0 && e20(x, y) >= 0; };

    for (int x = minx; x <= maxx; ++x) {
        for (int y = miny; y <= maxy; ++y) {
            if (!insideTriangle(x, y)) continue;

            screen.setPixelColor(size_t(x), size_t(y), color);
        }
    }
}

void Triangle::update(float dt) {}

}  // namespace eng
