#pragma once

#include <array>
#include <algorithm>
#include <execution>
#include <iterator>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "InterpolatableTuple.h"
#include "Light.h"
#include "Types.h"
#include "ProjectionInfo.h"
#include "Buffer.h"

namespace eng {

template <typename Buffer>
void drawLine(glm::vec3 pos1, glm::vec3 pos2, Color32 color, Buffer& buffer) {
    glm::vec3 p1 = pos1;
    glm::vec3 p2 = pos2;

    int x0 = int(buffer.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y0 = int(buffer.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x1 = int(buffer.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y1 = int(buffer.getHeight() * (p2.y + 1.0f) / 2.0f);

    int x = x0;
    int y = y0;
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy; /* error value e_xy */
    while (true) {     /* loop */
        float t = (x0 == x1 && y0 == y1)
                      ? 0.0f
                      : std::sqrt(static_cast<float>(std::pow(x - x0, 2) + std::pow(y - y0, 2)) / (std::pow(x1 - x0, 2) + std::pow(y1 - y0, 2)));
        float z = t * p2.z + (1 - t) * p1.z;

        if (z >= -1.0f && z <= 1.0f && x >= 0 && y >= 0 && x < buffer.getWidth() && y < buffer.getHeight()) {
            buffer.setPixelColor(size_t(x), size_t(y), Buffer::template convertColor<Color32, Buffer::Color>(color), z);
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

template <typename Var>
struct Triangle {
    glm::vec4 p0, p1, p2;
    Var v0, v1, v2;
};

float segmentPlaneIntersect(glm::vec3 point, glm::vec3 normal, glm::vec3 v1, glm::vec3 v2);

template <typename Var>
std::vector<Triangle<Var>> clipTriangleAgainstPlane(const Triangle<Var>& t, glm::vec3 point, glm::vec3 normal) {
    normal = glm::normalize(normal);

    auto distance = [&](glm::vec3 v) { return glm::dot(v - point, normal); };

    using Point = std::pair<glm::vec4, Var>;

    int ip = 0;
    std::array<Point, 3> insidePoints = {};
    int op = 0;
    std::array<Point, 3> outsidePoints = {};

    // neeeded for the correctness of resulting triangles normals
    bool oneInsideInvert = false;
    bool oneOutsideInvert = false;

    if (distance(t.p0) >= 0.0f) {
        insidePoints[ip++] = {t.p0, t.v0};
    } else {
        outsidePoints[op++] = {t.p0, t.v0};
    }

    if (distance(t.p1) >= 0.0f) {
        oneInsideInvert = true;
        insidePoints[ip++] = {t.p1, t.v1};
    } else {
        oneOutsideInvert = true;
        outsidePoints[op++] = {t.p1, t.v1};
    }

    if (distance(t.p2) >= 0.0f) {
        insidePoints[ip++] = {t.p2, t.v2};
    } else {
        outsidePoints[op++] = {t.p2, t.v2};
    }

    // triangle is full outside
    if (ip == 0) return {};

    // no clipping required, triangle is fully inside
    if (ip == 3) return {t};

    // need to clip, this will result with one smaller triangle
    if (ip == 1) {
        Triangle<Var> out;
        out.p0 = insidePoints[0].first;
        out.v0 = insidePoints[0].second;

        int i1 = 0;
        int i2 = 1;
        if (oneInsideInvert) std::swap(i1, i2);

        const auto& p0 = out.p0;
        const auto& p1 = outsidePoints[i1].first;
        const auto& p2 = outsidePoints[i2].first;

        float t = segmentPlaneIntersect(point, normal, p0, p1);

        out.p1 = p1 * t + p0 * (1.0f - t);
        out.v1 = outsidePoints[i1].second * t + out.v0 * (1.0f - t);

        float t2 = segmentPlaneIntersect(point, normal, p0, p2);

        out.p2 = p2 * t2 + p0 * (1.0f - t2);
        out.v2 = outsidePoints[i2].second * t2 + out.v0 * (1.0f - t2);

        return {out};
    }
    // need to clip, this will result in a quad (2 triangles)
    if (ip == 2) {
        Triangle<Var> out1, out2;

        int i1 = 0;
        int i2 = 1;
        if (oneOutsideInvert) std::swap(i1, i2);

        const auto& p0 = insidePoints[i1].first;
        const auto& p1 = insidePoints[i2].first;
        const auto& p2 = outsidePoints[0].first;

        out1.p0 = p0;
        out1.v0 = insidePoints[i1].second;
        out1.p1 = p1;
        out1.v1 = insidePoints[i2].second;

        float t = segmentPlaneIntersect(point, normal, p0, p2);
        out1.p2 = p2 * t + p0 * (1.0f - t);
        out1.v2 = outsidePoints[0].second * t + insidePoints[i1].second * (1.0f - t);

        out2.p0 = out1.p1;
        out2.v0 = out1.v1;
        out2.p2 = out1.p2;
        out2.v2 = out1.v2;

        float t2 = segmentPlaneIntersect(point, normal, p1, p2);
        out2.p1 = p2 * t2 + p1 * (1.0f - t2);
        out2.v1 = outsidePoints[0].second * t2 + insidePoints[i2].second * (1.0f - t2);

        return {out1, out2};
    }
    return {};
}

template <typename Var>
std::vector<Triangle<Var>> clipTriangleAgainstFrustrum(const Triangle<Var>& t, const glm::mat4& projection, float near) {
    Triangle<Var> tr = t;

    // coordinates are (x, y, z, 1) in camera space

    std::vector<Triangle<Var>> trianglesToDraw = clipTriangleAgainstPlane(tr, {0, 0, -near - 0.01f}, {0, 0, -1});

    for (auto& triangle : trianglesToDraw) {
        // for perspective-correct interpolation
        triangle.v0 *= 1.0f / triangle.p0.z;
        triangle.v1 *= 1.0f / triangle.p1.z;
        triangle.v2 *= 1.0f / triangle.p2.z;

        glm::vec4 p0 = projection * triangle.p0;
        glm::vec4 p1 = projection * triangle.p1;
        glm::vec4 p2 = projection * triangle.p2;

        // coordinates are now (x', y', z', 1/z)
        triangle.p0 = glm::vec4(glm::vec3(p0 / p0.w), 1.0f / -p0.w);
        triangle.p1 = glm::vec4(glm::vec3(p1 / p1.w), 1.0f / -p1.w);
        triangle.p2 = glm::vec4(glm::vec3(p2 / p2.w), 1.0f / -p2.w);
    }

    std::vector<glm::vec3> points = {{-1, 0, 0}, {1, 0, 0}, {0, -1, 0}, {0, 1, 0}};
    std::vector<glm::vec3> normals = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}};

    for (size_t i = 0; i < points.size(); ++i) {
        std::vector<Triangle<Var>> newTriangles = {};

        for (const auto& triangle : trianglesToDraw) {
            auto clipped = clipTriangleAgainstPlane(triangle, points[i], normals[i]);

            newTriangles.insert(newTriangles.end(), std::make_move_iterator(clipped.begin()), std::make_move_iterator(clipped.end()));
        }

        trianglesToDraw = std::move(newTriangles);
    }

    return trianglesToDraw;
}

template <typename Buffer>
void drawWireframeTriangle(const Triangle<NoVariables>& t, const glm::mat4& projection, Color32 color, ProjectionInfo& info, Buffer& buffer) {
    float near = info.getNearPlaneDistance();
    for (const auto& triangle : clipTriangleAgainstFrustrum(t, projection, near)) {
        drawLine(triangle.p0, triangle.p1, color, buffer);
        drawLine(triangle.p1, triangle.p2, color, buffer);
        drawLine(triangle.p2, triangle.p0, color, buffer);
    }
}

template <typename Var, typename Shader, typename Buffer>
void drawTriangle(const Triangle<Var>& t, const glm::mat4& projection, Shader& shader, ProjectionInfo& info, const LightsVec& lights, Buffer& buffer) {
    float near = info.getNearPlaneDistance();
    for (const auto& triangle : clipTriangleAgainstFrustrum(t, projection, near)) {
        rasterizeTriangle(triangle, shader, buffer, lights);
    }
}

template <typename Var, typename Shader, typename Buffer>
void rasterizeTriangle(const Triangle<Var>& t, Shader& shader, Buffer& buffer, const LightsVec& lights) {
    glm::vec4 p0 = t.p0;
    glm::vec4 p1 = t.p1;
    glm::vec4 p2 = t.p2;

    int x0 = int(buffer.getWidth() * (p0.x + 1.0f) / 2.0f);
    int y0 = int(buffer.getHeight() * (p0.y + 1.0f) / 2.0f);

    int x1 = int(buffer.getWidth() * (p1.x + 1.0f) / 2.0f);
    int y1 = int(buffer.getHeight() * (p1.y + 1.0f) / 2.0f);

    int x2 = int(buffer.getWidth() * (p2.x + 1.0f) / 2.0f);
    int y2 = int(buffer.getHeight() * (p2.y + 1.0f) / 2.0f);

    int minx = std::min({x0, x1, x2});
    int miny = std::min({y0, y1, y2});
    int maxx = std::max({x0, x1, x2});
    int maxy = std::max({y0, y1, y2});

    auto e01 = [&](int x, int y) { return (y - y0) * (x1 - x0) - (x - x0) * (y1 - y0); };
    auto e12 = [&](int x, int y) { return (y - y1) * (x2 - x1) - (x - x1) * (y2 - y1); };
    auto e20 = [&](int x, int y) { return (y - y2) * (x0 - x2) - (x - x2) * (y0 - y2); };

    auto insideTriangle = [&](int x, int y) { return e01(x, y) >= 0 && e12(x, y) >= 0 && e20(x, y) >= 0; };

    const auto& t0 = t.v0;
    const auto& t1 = t.v1;
    const auto& t2 = t.v2;

    int xMin = std::max(0, minx);
    int xMax = std::min(static_cast<int>(buffer.getWidth()) - 1, maxx);

    int yMin = std::max(0, miny);
    int yMax = std::min(static_cast<int>(buffer.getHeight()) - 1, maxy);

    for (int x = xMin; x <= xMax; ++x) {
        for (int y = yMin; y <= yMax; ++y) {
            if (!insideTriangle(x, y)) continue;

            float w0 = e12(x, y);
            float w1 = e20(x, y);
            float w2 = e01(x, y);
            float l = w0 + w1 + w2;

            if (l <= 0.0001f) continue;

            w0 /= l;
            w1 /= l;
            w2 /= l;
            float z = w0 * p0.z + w1 * p1.z + w2 * p2.z;

            float w = w0 * p0.w + w1 * p1.w + w2 * p2.w;

            if (z < -1 || z > 1) continue;
            if (z > buffer.getPixelDepth(Pixels(x), Pixels(y))) continue;

            Var t = (t0 * w0 + t1 * w1 + t2 * w2) * (1.0f / w);
            auto lighting = shader.computePixelColor(t, lights);
            if (!Buffer::template discard<Color128>(lighting)) {
                auto destination = Buffer::template convertColor<Buffer::Color, Color128>(buffer.getPixelColor(Pixels(x), Pixels(y)));
                auto blended = Buffer::template blend<Color128>(lighting, destination);
                buffer.setPixelColor(Pixels(x), Pixels(y), Buffer::template convertColor<Color128, Buffer::Color>(blended), z);
            }
        }
    }
}

}  // namespace eng
