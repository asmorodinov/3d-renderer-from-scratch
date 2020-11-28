#include "Object.h"

namespace eng {

Transform& Object::getTransform() { return t; }

Point::Point(glm::vec3 position, ColorType color, unsigned pixelRadius) : color(color), pixelRadius(pixelRadius) {
    t.position = position;
}

void Point::draw(const Camera& camera, Screen& screen) {
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

void Line::draw(const Camera& camera, Screen& screen) {
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

Texture Triangle::texture = Texture("texture.png");
Triangle::Triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color, bool s)
    : color(color), pos1(pos1), pos2(pos2), pos3(pos3), s(s) {}

void Triangle::draw(const Camera& camera, Screen& screen) {
    glm::vec4 p0 = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), pos1);
    glm::vec4 p1 = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), pos2);
    glm::vec4 p2 = screen.localSpaceToScreenSpace(camera.getViewMatrix(), t.getModel(), pos3);

    glm::vec3 p0w = t.getModel() * glm::vec4(pos1, 1.0f);
    glm::vec3 p1w = t.getModel() * glm::vec4(pos2, 1.0f);
    glm::vec3 p2w = t.getModel() * glm::vec4(pos3, 1.0f);

    glm::vec3 l1 = p1w - p0w;
    glm::vec3 l2 = p2w - p0w;
    glm::vec3 normal = -glm::normalize(glm::cross(l1, l2));
    glm::vec3 cameraRay = p1w - camera.getPosition();
    if (glm::dot(normal, -cameraRay) < 0.0f) return;

    if (p0.x <= -1.0f || p0.x >= 1.0f || p0.y <= -1.0f || p0.y >= 1.0f) return;
    if (p1.x <= -1.0f || p1.x >= 1.0f || p1.y <= -1.0f || p1.y >= 1.0f) return;
    if (p2.x <= -1.0f || p2.x >= 1.0f || p2.y <= -1.0f || p2.y >= 1.0f) return;

    int x0 = int(screen.getWidth() * (p0.x + 1.0f) / 2.0f);
    int y0 = int(screen.getHeight() * (p0.y + 1.0f) / 2.0f);

    int x1 = int(screen.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y1 = int(screen.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x2 = int(screen.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y2 = int(screen.getHeight() * (p2.y + 1.0f) / 2.0f);

    int minx = std::min({x0, x1, x2});
    int miny = std::min({y0, y1, y2});
    int maxx = std::max({x0, x1, x2});
    int maxy = std::max({y0, y1, y2});

    auto e01 = [&](int x, int y) { return (x - x0) * (y1 - y0) - (y - y0) * (x1 - x0); };
    auto e12 = [&](int x, int y) { return (x - x1) * (y2 - y1) - (y - y1) * (x2 - x1); };
    auto e20 = [&](int x, int y) { return (x - x2) * (y0 - y2) - (y - y2) * (x0 - x2); };

    auto insideTriangle = [&](int x, int y) { return e01(x, y) >= 0 && e12(x, y) >= 0 && e20(x, y) >= 0; };

    glm::vec2 t0, t1, t2;
    if (!s) {
        t0 = {0, 0};
        t1 = {1, 0};
        t2 = {0, 1};
    } else {
        t0 = {1, 1};
        t1 = {0, 1};
        t2 = {1, 0};
    }
    t0 /= p0.w;
    t1 /= p1.w;
    t2 /= p2.w;

    for (int x = minx; x <= maxx; ++x) {
        for (int y = miny; y <= maxy; ++y) {
            if (!insideTriangle(x, y)) continue;

            float w0 = e12(x, y);
            float w1 = e20(x, y);
            float w2 = e01(x, y);
            float l = w0 + w1 + w2;
            w0 /= l;
            w1 /= l;
            w2 /= l;
            float z = 1.0f / (w0 / p0.z + w1 / p1.z + w2 / p2.z);
            float w = 1.0f / (w0 / p0.w + w1 / p1.w + w2 / p2.w);

            glm::vec2 t = w * (w0 * t0 + w1 * t1 + w2 * t2);
            glm::vec3 color1 = texture.sample(t.s, t.t);
            glm::vec3 lighting = color1 * 0.0f;

            glm::vec3 FragPos = w * (w0 * p0w / p0.w + w1 * p1w / p1.w + w2 * p2w / p2.w);
            glm::vec3 viewPos = camera.getPosition();
            glm::vec3 lightPos = glm::vec3(2, 1, 0);
            glm::vec3 viewDir = glm::normalize(viewPos - FragPos);
            glm::vec3 lightDir = glm::normalize(lightPos - FragPos);
            glm::vec3 diffuse = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.5f) * color1 * glm::vec3(1.0f) * 1.0f;
            glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
            float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
            glm::vec3 specular = glm::vec3(1.0f) * spec;

            float d = length(lightPos - FragPos);
            float attenuation = 1.0 / (1.0 + 0.00001 * d * d * d);
            diffuse *= attenuation / 1.7;
            specular *= attenuation / 1.7;
            lighting += glm::vec3(1.0f) * (diffuse + specular);

            screen.setPixelColor(size_t(x), size_t(y), lighting, z);
        }
    }
}

void Triangle::update(float dt) {}

}  // namespace eng
