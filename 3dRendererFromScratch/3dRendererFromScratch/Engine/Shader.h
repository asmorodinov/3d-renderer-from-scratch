#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Texture.h"

namespace eng {

template <class T>
concept Interpolatable = requires(T s, T e, T r, float t) {
    r = s * (1.0f - t) + e * t;
    s *= t;
    s = T();
};

template <typename Const, Interpolatable Var>
class Shader {
 public:
    Shader() : c(), shader() {}

    Const& getConst() { return c; }
    void setConst(const Const& c1) { c = c1; }

    auto& getShader() { return shader; }

 private:
    std::function<glm::vec3(const Const&, const Var&, const LightsVec&)> shader;
    Const c;
};

struct PhongVar {
    glm::vec3 pos;
    glm::vec2 uv;

    void operator*=(float t) {
        pos *= t;
        uv *= t;
    }
    PhongVar operator*(float t) const { return {pos * t, uv * t}; }
    PhongVar operator+(const PhongVar& oth) const { return {pos + oth.pos, uv + oth.uv}; }
};
struct PhongConst {
    Texture t;
    glm::vec3 viewPos;
    glm::vec3 normal;
};
using PhongShader = Shader<PhongConst, PhongVar>;

struct FlatVar {
    FlatVar() {}

    void operator*=(float t) {}
    FlatVar operator*(float t) const { return {}; }
    FlatVar operator+(const FlatVar& oth) const { return {}; }
};
struct FlatConst {
    glm::vec3 color;
};
using FlatShader = Shader<FlatConst, FlatVar>;

}  // namespace eng
