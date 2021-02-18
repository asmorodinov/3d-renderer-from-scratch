#pragma once

#include <algorithm>
#include <execution>
#include <iterator>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Screen.h"
#include "Shader.h"

namespace eng {

// It's hard to see whether parallel for actually gives any perfomance benefits or not
// #define PARALLEL

void drawLine(glm::vec3 pos1, glm::vec3 pos2, ColorType color, Screen& screen);

struct Triangle {
    glm::vec4 p0, p1, p2;
    ShaderVariablesVec v0, v1, v2;
};

std::vector<Triangle> clipTriangleAgainstFrustrum(const Triangle& t, const glm::mat4& projection, float near);

void drawWireframeTriangle(const Triangle& t, const glm::mat4& projection, float near, ColorType color, Screen& screen);

void drawTriangle(const Triangle& t, const glm::mat4& projection, float near, Shader& shader, Screen& screen,
                  const LightsVec& lights);

void rasterizeTriangle(const Triangle& t, Shader& shader, Screen& screen, const LightsVec& lights);

}  // namespace eng
