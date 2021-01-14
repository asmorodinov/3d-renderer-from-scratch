#pragma once

#include <assert.h>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Texture.h"

namespace eng {

class ShaderVariable {
 public:
    using var_t = std::variant<glm::vec4, glm::vec3, glm::vec2, float>;

    template <typename T>
    ShaderVariable(const T& v) : variable(v) {}

    template <typename T>
    void set(const T& v) {
        variable = v;
    }
    template <typename T>
    T get() const {
        return std::get<T>(variable);
    }

    void operator*=(float t);
    ShaderVariable operator*(float t) const;
    ShaderVariable operator+(const ShaderVariable& other) const;

 private:
    var_t variable;
};

class ShaderVariablesVec {
 public:
    ShaderVariablesVec();
    ShaderVariablesVec(const std::vector<ShaderVariable>& vec);

    ShaderVariable& operator[](size_t i);
    const ShaderVariable& operator[](size_t i) const;

    void operator*=(float t);
    ShaderVariablesVec operator*(float t) const;
    ShaderVariablesVec operator+(const ShaderVariablesVec& other) const;

 protected:
    std::vector<ShaderVariable> vec;
};

class ShaderConstant {
 public:
    using var_t = std::variant<CubemapTexture*, Texture*, glm::vec4, glm::vec3, glm::vec2, float, int>;

    template <typename T>
    ShaderConstant(const T& v) : variable(v) {}

    template <typename T>
    void set(const T& v) {
        variable = v;
    }
    template <typename T>
    T get() const {
        return std::get<T>(variable);
    }

 private:
    var_t variable;
};
using ShaderConstantsVec = std::vector<ShaderConstant>;

class Shader {
 public:
    using Const = ShaderConstantsVec;
    using Var = ShaderVariablesVec;

    Shader() : c(), shader() {}

    Const& getConst();
    void setConst(const Const& c1);

    using Func = std::function<glm::vec4(const Const&, const Var&, const LightsVec&)>;

    const Func& getShader() const;
    void setShader(const Func& f);

 private:
    Func shader;
    Const c;
};

}  // namespace eng
