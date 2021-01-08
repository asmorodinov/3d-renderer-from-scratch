#pragma once

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Screen.h"
#include "Shader.h"

namespace eng {

void drawLine(glm::vec4 pos1, glm::vec4 pos2, ColorType color, Screen& screen);

struct Triangle {
    glm::vec4 p0, p1, p2;
    ShaderVariablesVec v0, v1, v2;
};

std::vector<Triangle> clipTriangleAgainstFrustrum(const Triangle& t);

void drawWireframeTriangle(const Triangle& t, ColorType color, Screen& screen);

void drawTriangle(const Triangle& t, Shader& shader, Screen& screen, const LightsVec& lights);

void drawTriangleNormalVersion(const Triangle& t, Shader& shader, Screen& screen, const LightsVec& lights);

struct InterpolatedVariables {
    float z;
    float w;
    ShaderVariablesVec t;

    void operator*=(float f);
    InterpolatedVariables operator*(float f) const;
    InterpolatedVariables operator+(const InterpolatedVariables& oth) const;
};

// approximately 20% faster than the normal version for the flat color shader, about 20% slower for the phong shader
void drawTriangleOvercomplicatedVersion(const Triangle& t, Shader& shader, Screen& screen, const LightsVec& lights);

}  // namespace eng
