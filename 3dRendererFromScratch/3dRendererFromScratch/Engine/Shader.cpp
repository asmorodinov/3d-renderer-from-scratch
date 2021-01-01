#include "Shader.h"

namespace eng {

void ShaderVariable::operator*=(float t) {
    std::visit([t](auto&& arg) { arg *= t; }, variable);
}
ShaderVariable ShaderVariable::operator*(float t) const {
    return ShaderVariable(std::visit([t](auto&& arg) -> var_t { return arg * t; }, variable));
}
ShaderVariable ShaderVariable::operator+(const ShaderVariable& other) const {
    return ShaderVariable(std::visit(
        [&other](auto&& arg) -> var_t {
            using T = std::decay_t<decltype(arg)>;
            return arg + other.get<T>();
        },
        variable));
}

ShaderVariablesVec::ShaderVariablesVec() : vec() {}
ShaderVariablesVec::ShaderVariablesVec(const std::vector<ShaderVariable>& vec) : vec(vec) {}

ShaderVariable& ShaderVariablesVec::operator[](size_t i) { return vec[i]; }
const ShaderVariable& ShaderVariablesVec::operator[](size_t i) const { return vec[i]; }

void ShaderVariablesVec::operator*=(float t) {
    for (auto& v : vec) v *= t;
}
ShaderVariablesVec ShaderVariablesVec::operator*(float t) const {
    ShaderVariablesVec res = *this;
    res *= t;
    return res;
}
ShaderVariablesVec ShaderVariablesVec::operator+(const ShaderVariablesVec& other) const {
    assert(other.vec.size() == vec.size());
    ShaderVariablesVec res = *this;
    for (size_t i = 0; i < vec.size(); ++i) res[i] = res[i] + other[i];
    return res;
}

Shader::Const& Shader::getConst() { return c; }
void Shader::setConst(const Shader::Const& c1) { c = c1; }

const Shader::Func& Shader::getShader() const { return shader; }
void Shader::setShader(const Func& f) { shader = f; }

}  // namespace eng
