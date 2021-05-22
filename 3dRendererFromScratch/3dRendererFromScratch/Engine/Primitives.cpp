#include "Primitives.h"

namespace eng {

void drawLine(glm::vec3 pos1, glm::vec3 pos2, ColorType color, Screen& screen) {
    glm::vec3 p1 = pos1;
    glm::vec3 p2 = pos2;

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

        if (z >= -1.0f && z <= 1.0f && x >= 0 && y >= 0 && x < screen.getWidth() && y < screen.getHeight()) {
            screen.setPixelColor(size_t(x), size_t(y), color, z);
        }

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

float segmentPlaneIntersect(glm::vec3 point, glm::vec3 normal, glm::vec3 v1, glm::vec3 v2) {
    normal = glm::normalize(normal);

    float dot = glm::dot(point, normal);
    float dot1 = glm::dot(v1, normal);
    float dot2 = glm::dot(v2, normal);

    return (dot - dot1) / (dot2 - dot1);
}

void drawWireframeTriangle(const Triangle<NoVariables>& t, const glm::mat4& projection, ColorType color, Screen& screen) {
    float near = screen.getNearPlaneDistance();
    for (const auto& triangle : clipTriangleAgainstFrustrum(t, projection, near)) {
        drawLine(triangle.p0, triangle.p1, color, screen);
        drawLine(triangle.p1, triangle.p2, color, screen);
        drawLine(triangle.p2, triangle.p0, color, screen);
    }
}

}  // namespace eng
