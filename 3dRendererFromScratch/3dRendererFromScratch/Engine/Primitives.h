#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Screen.h"
#include "Shader.h"

namespace eng {

template <InterpolatableAndSwapable Var>
struct Triangle {
    glm::vec3 p0, p1, p2;
    Var v0, v1, v2;
};

using PhongTriangle = Triangle<PhongVar>;
using FlatTriangle = Triangle<FlatVar>;

template <typename Const, InterpolatableAndSwapable Var>
void drawTriangle(const Triangle<Var>& t, const glm::mat4& transform, Shader<Const, Var>& shader, Screen& screen,
                  const LightsVec& lights) {
    glm::vec4 p0 = transform * glm::vec4(t.p0, 1.0f);
    glm::vec4 p1 = transform * glm::vec4(t.p1, 1.0f);
    glm::vec4 p2 = transform * glm::vec4(t.p2, 1.0f);
    p0 = glm::vec4(p0.x / p0.w, p0.y / p0.w, p0.z / p0.w, p0.w);
    p1 = glm::vec4(p1.x / p1.w, p1.y / p1.w, p1.z / p1.w, p1.w);
    p2 = glm::vec4(p2.x / p2.w, p2.y / p2.w, p2.z / p2.w, p2.w);

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

    Var t0 = t.v0;
    Var t1 = t.v1;
    Var t2 = t.v2;
    t0 *= 1.0f / p0.w;
    t1 *= 1.0f / p1.w;
    t2 *= 1.0f / p2.w;

    auto& shaderFunc = shader.getShader();

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
            if (z > screen.getPixelDepth(size_t(x), size_t(y))) continue;

            float w = 1.0f / (w0 / p0.w + w1 / p1.w + w2 / p2.w);

            Var t = (t0 * w0 + t1 * w1 + t2 * w2) * w;
            auto lighting = shaderFunc(shader.getConst(), t, lights);

            screen.setPixelColor(size_t(x), size_t(y), lighting, z);
        }
    }
}

}  // namespace eng
