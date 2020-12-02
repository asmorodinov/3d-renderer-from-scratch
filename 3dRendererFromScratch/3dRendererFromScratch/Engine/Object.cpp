#include "Object.h"

namespace eng {

Transform& Object::getTransform() { return t; }

Point::Point(glm::vec3 position, ColorType color, unsigned pixelRadius) : color(color), pixelRadius(pixelRadius) {
    t.position = position;
}

void Point::draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
    glm::vec4 screenCoords = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), glm::vec3(0.0f));

    if (screenCoords.x <= -1.0f || screenCoords.x >= 1.0f || screenCoords.y <= -1.0f || screenCoords.y >= 1.0f) return;

    int x = int(screen.getWidth() * (screenCoords.x + 1.0f) / 2.0f);
    int y = int(screen.getHeight() * (screenCoords.y + 1.0f) / 2.0f);

    for (int i = x - pixelRadius; i <= x + pixelRadius; ++i)
        for (int j = y - pixelRadius; j <= y + pixelRadius; ++j) {
            if (i < 0 || j < 0) continue;

            screen.setPixelColor(size_t(i), size_t(j), color, screenCoords.z);
        }
}

void Point::update(float dt) {}

Line::Line(glm::vec3 pos1, glm::vec3 pos2, ColorType color) : color(color), pos1(pos1), pos2(pos2) {}

void Line::draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
    glm::vec4 p1 = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), pos1);
    glm::vec4 p2 = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), pos2);

    if (p1.x <= -1.0f || p1.x >= 1.0f || p1.y <= -1.0f || p1.y >= 1.0f) return;
    if (p2.x <= -1.0f || p2.x >= 1.0f || p2.y <= -1.0f || p2.y >= 1.0f) return;

    int x0 = int(screen.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y0 = int(screen.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x1 = int(screen.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y1 = int(screen.getHeight() * (p2.y + 1.0f) / 2.0f);

    int x = x0;
    int y = y0;
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy; /* error value e_xy */
    while (true) {     /* loop */
        float t = (x0 == x1 && y0 == y1) ? 0.0f
                                         : std::sqrt(static_cast<float>(std::pow(x - x0, 2) + std::pow(y - y0, 2)) /
                                                     (std::pow(x1 - x0, 2) + std::pow(y1 - y0, 2)));
        float z = t * p2.z + (1 - t) * p1.z;

        screen.setPixelColor(size_t(x), size_t(y), color, z);
        if (x == x1 && y == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) { /* e_xy+e_x > 0 */
            err += dy;
            x += sx;
        }
        if (e2 <= dx) { /* e_xy+e_y < 0 */
            err += dx;
            y += sy;
        }
    }
}

void Line::update(float dt) {}

TriangleObj::TriangleObj(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color, PhongShader& ph, FlatShader& fl, bool s,
                         bool flat)
    : color(color), pos1(pos1), pos2(pos2), pos3(pos3), ph(ph), fl(fl), s(s), flat(flat) {
    if (!flat) {
        if (!s) {
            pht.v0.uv = {0, 0};
            pht.v1.uv = {1, 0};
            pht.v2.uv = {0, 1};
        } else {
            pht.v0.uv = {1, 1};
            pht.v1.uv = {0, 1};
            pht.v2.uv = {1, 0};
        }
    }
}

void TriangleObj::draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
    glm::mat4 model = t.getModel();
    glm::vec4 p0_ = model * glm::vec4(pos1, 1.0f);
    glm::vec4 p1_ = model * glm::vec4(pos2, 1.0f);
    glm::vec4 p2_ = model * glm::vec4(pos3, 1.0f);
    p0_ /= p0_.w;
    p1_ /= p1_.w;
    p2_ /= p2_.w;
    glm::vec3 p0 = glm::vec3(p0_);
    glm::vec3 p1 = glm::vec3(p1_);
    glm::vec3 p2 = glm::vec3(p2_);

    glm::vec3 normal = -glm::normalize(glm::cross(p1 - p0, p2 - p0));

    glm::mat4 transform = screen.getProjectionMatrix() * camera.getViewMatrix();

    if (!flat) {
        pht.v0.pos = p0;
        pht.v1.pos = p1;
        pht.v2.pos = p2;

        pht.p0 = p0;
        pht.p1 = p1;
        pht.p2 = p2;

        ph.getConst().normal = normal;
        ph.getConst().viewPos = camera.getPosition();

        drawTriangle(pht, transform, ph, screen, lights);
    } else {
        flt.p0 = p0;
        flt.p1 = p1;
        flt.p2 = p2;
        fl.getConst().color = color;

        drawTriangle(flt, transform, fl, screen, lights);
    }
}

void TriangleObj::update(float dt) {}

}  // namespace eng
