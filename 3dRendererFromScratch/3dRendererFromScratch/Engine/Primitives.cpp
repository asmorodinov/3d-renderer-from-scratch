#include "Primitives.h"

namespace eng {

void drawLine(glm::vec3 pos1, glm::vec3 pos2, ColorType color, const glm::mat4& transform, Screen& screen) {
    glm::vec4 p1 = transform * glm::vec4(pos1, 1.0f);
    glm::vec4 p2 = transform * glm::vec4(pos2, 1.0f);
    p1 = glm::vec4(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w, p1.w);
    p2 = glm::vec4(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w, p2.w);

    if ((p1.x < -1.0f && p2.x < -1.0f) || (p1.x > 1.0f && p2.x > 1.0f) || (p1.y < -1.0f && p2.y < -1.0f) ||
        (p1.y > 1.0f && p2.y > 1.0f))
        return;

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
        float z = 1.0f / (t / p2.z + (1 - t) / p1.z);

        if (x >= 0 && y >= 0 && x < screen.getWidth() && y < screen.getHeight()) {
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

void InterpolatedVariables::operator*=(float f) {
    z *= f;
    w *= f;
    t *= f;
}
InterpolatedVariables InterpolatedVariables::operator*(float f) const { return {z * f, w * f, t * f}; }
InterpolatedVariables InterpolatedVariables::operator+(const InterpolatedVariables& oth) const {
    return {z + oth.z, w + oth.w, t + oth.t};
}

void drawTriangleNormalVersion(const Triangle& t, const glm::mat4& transform, Shader& shader, Screen& screen,
                               const LightsVec& lights) {
    glm::vec4 p0 = transform * glm::vec4(t.p0, 1.0f);
    glm::vec4 p1 = transform * glm::vec4(t.p1, 1.0f);
    glm::vec4 p2 = transform * glm::vec4(t.p2, 1.0f);
    p0 = glm::vec4(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w, p0.w);
    p1 = glm::vec4(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w, p1.w);
    p2 = glm::vec4(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w, p2.w);

    if ((p0.x < -1.0f && p1.x < -1.0f && p2.x < -1.0f) || (p0.x > 1.0f && p1.x > 1.0f && p2.x > 1.0f) ||
        (p0.y < -1.0f && p1.y < -1.0f && p2.y < -1.0f) || (p0.y > 1.0f && p1.y > 1.0f && p2.y > 1.0f))
        return;

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

    using Var = ShaderVariablesVec;
    Var t0 = t.v0;
    Var t1 = t.v1;
    Var t2 = t.v2;
    t0 *= 1.0f / p0.w;
    t1 *= 1.0f / p1.w;
    t2 *= 1.0f / p2.w;

    auto& shaderFunc = shader.getShader();

    for (int x = std::max(0, minx); x <= std::min(static_cast<int>(screen.getWidth()) - 1, maxx); ++x) {
        for (int y = std::max(0, miny); y <= std::min(static_cast<int>(screen.getHeight()) - 1, maxy); ++y) {
            if (!insideTriangle(x, y)) continue;

            float w0 = e12(x, y);
            float w1 = e20(x, y);
            float w2 = e01(x, y);
            float l = w0 + w1 + w2;
            w0 /= l;
            w1 /= l;
            w2 /= l;
            float z = 1.0f / (w0 / p0.z + w1 / p1.z + w2 / p2.z);
            if (z > screen.getPixelDepth(size_t(x), size_t(y))) continue;

            float w = 1.0f / (w0 / p0.w + w1 / p1.w + w2 / p2.w);
            if (z < -1.0f || z > 1.0f || w < 0.0f) continue;

            Var t = (t0 * w0 + t1 * w1 + t2 * w2) * w;
            auto lighting = shaderFunc(shader.getConst(), t, lights);
            if (lighting.a == 0.0f) continue;

            screen.setPixelColor(size_t(x), size_t(y), glm::vec3(lighting.r, lighting.g, lighting.b), z);
        }
    }
}

void drawTriangle(const Triangle& t, const glm::mat4& transform, Shader& shader, Screen& screen, const LightsVec& lights) {
    // drawTriangleOvercomplicatedVersion(t, transform, shader, screen, lights);
    drawTriangleNormalVersion(t, transform, shader, screen, lights);
}

void drawTriangleOvercomplicatedVersion(const Triangle& t, const glm::mat4& transform, Shader& shader, Screen& screen,
                                        const LightsVec& lights) {
    glm::vec4 p0 = transform * glm::vec4(t.p0, 1.0f);
    glm::vec4 p1 = transform * glm::vec4(t.p1, 1.0f);
    glm::vec4 p2 = transform * glm::vec4(t.p2, 1.0f);
    p0 = glm::vec4(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w, p0.w);
    p1 = glm::vec4(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w, p1.w);
    p2 = glm::vec4(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w, p2.w);

    if ((p0.x < -1.0f && p1.x < -1.0f && p2.x < -1.0f) || (p0.x > 1.0f && p1.x > 1.0f && p2.x > 1.0f) ||
        (p0.y < -1.0f && p1.y < -1.0f && p2.y < -1.0f) || (p0.y > 1.0f && p1.y > 1.0f && p2.y > 1.0f))
        return;

    int x0 = int(screen.getWidth() * (p0.x + 1.0f) / 2.0f);
    int y0 = int(screen.getHeight() * (p0.y + 1.0f) / 2.0f);

    int x1 = int(screen.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y1 = int(screen.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x2 = int(screen.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y2 = int(screen.getHeight() * (p2.y + 1.0f) / 2.0f);

    auto& shaderFunc = shader.getShader();

    using Var = ShaderVariablesVec;
    Var t0 = t.v0;
    Var t1 = t.v1;
    Var t2 = t.v2;
    t0 *= 1.0f / p0.w;
    t1 *= 1.0f / p1.w;
    t2 *= 1.0f / p2.w;

    if (y1 < y0) {
        std::swap(x0, x1);
        std::swap(y0, y1);
        std::swap(p0, p1);
        std::swap(t0, t1);
    }
    if (y2 < y0) {
        std::swap(x0, x2);
        std::swap(y0, y2);
        std::swap(p0, p2);
        std::swap(t0, t2);
    }
    if (y2 < y1) {
        std::swap(x1, x2);
        std::swap(y1, y2);
        std::swap(p1, p2);
        std::swap(t1, t2);
    }

    InterpolatedVariables iv0 = {1.0f / p0.z, 1.0f / p0.w, t0};
    InterpolatedVariables iv1 = {1.0f / p1.z, 1.0f / p1.w, t1};
    InterpolatedVariables iv2 = {1.0f / p2.z, 1.0f / p2.w, t2};

    int dx1 = x1 - x0;
    int dy1 = y1 - y0;
    int dy2 = y2 - y0;
    int dx2 = x2 - x0;

    auto div1 = iv1 + (iv0 * -1.0f);
    auto div2 = iv2 + (iv0 * -1.0f);

    auto iv = InterpolatedVariables();

    float ddax = 0.0f;
    float ddbx = 0.0f;
    auto ddiv1 = InterpolatedVariables();
    auto ddiv2 = InterpolatedVariables();

    if (dy1) {
        ddax = dx1 / static_cast<float>(dy1);
        ddiv1 = div1 * (1.0f / static_cast<float>(dy1));
    }
    if (dy2) {
        ddbx = dx2 / static_cast<float>(dy2);
        ddiv2 = div2 * (1.0f / static_cast<float>(dy2));
    }

    if (dy1 && 1) {
        for (int y = std::max(0, y0); y <= std::min(static_cast<int>(screen.getHeight()) - 1, y1); ++y) {
            int ax = x0 + (y - y0) * ddax;
            int bx = x0 + (y - y0) * ddbx;

            if (ax == bx) continue;

            auto siv = iv0 + ddiv1 * static_cast<float>(y - y0);
            auto eiv = iv0 + ddiv2 * static_cast<float>(y - y0);

            if (ax > bx) {
                std::swap(ax, bx);
                std::swap(siv, eiv);
            }

            for (int x = std::max(0, ax); x <= std::min(static_cast<int>(screen.getWidth()) - 1, bx); ++x) {
                float t = static_cast<float>(x - ax) / (bx - ax);

                iv = siv * (1.0f - t) + eiv * t;

                float w = 1.0f / iv.w;
                float z = 1.0f / iv.z;

                if (z > screen.getPixelDepth(size_t(x), size_t(y))) continue;

                Var it = iv.t * w;
                auto lighting = shaderFunc(shader.getConst(), it, lights);
                if (lighting.a == 0.0f) continue;

                screen.setPixelColor(size_t(x), size_t(y), glm::vec3(lighting.r, lighting.g, lighting.b), z);
            }
        }
    }

    dx1 = x2 - x1;
    dy1 = y2 - y1;
    div1 = iv2 + (iv1 * -1.0f);

    ddax = 0.0f;
    ddiv1 = InterpolatedVariables();

    if (dy1) {
        ddax = dx1 / static_cast<float>(dy1);
        ddiv1 = div1 * (1.0f / static_cast<float>(dy1));
    }

    if (dy1) {
        for (int y = std::max(0, y1); y <= std::min(static_cast<int>(screen.getHeight()) - 1, y2); ++y) {
            int ax = x1 + (y - y1) * ddax;
            int bx = x0 + (y - y0) * ddbx;

            if (ax == bx) continue;

            auto siv = iv1 + ddiv1 * static_cast<float>(y - y1);
            auto eiv = iv0 + ddiv2 * static_cast<float>(y - y0);

            if (ax > bx) {
                std::swap(ax, bx);
                std::swap(siv, eiv);
            }

            for (int x = std::max(0, ax); x <= std::min(static_cast<int>(screen.getWidth()) - 1, bx); ++x) {
                float t = static_cast<float>(x - ax) / (bx - ax);

                iv = siv * (1.0f - t) + eiv * t;

                float w = 1.0f / iv.w;
                float z = 1.0f / iv.z;

                if (z > screen.getPixelDepth(size_t(x), size_t(y))) continue;

                Var it = iv.t * w;
                auto lighting = shaderFunc(shader.getConst(), it, lights);
                if (lighting.a == 0.0f) continue;

                screen.setPixelColor(size_t(x), size_t(y), glm::vec3(lighting.r, lighting.g, lighting.b), z);
            }
        }
    }
}

}  // namespace eng
