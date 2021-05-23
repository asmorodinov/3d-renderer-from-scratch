#include "BasicShader.h"

namespace eng {

void BasicVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    model = model_;
    view = view_;
    projection = projection_;

    viewPos = viewPos_;

    vm = view * model;

    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
}

BasicVertexShader::BasicVertexShaderOutput BasicVertexShader::run(const WorldSpaceTriangle& tr) {
    glm::vec4 v0 = glm::vec4(tr.v0, 1.0f);
    glm::vec4 v1 = glm::vec4(tr.v1, 1.0f);
    glm::vec4 v2 = glm::vec4(tr.v2, 1.0f);

    glm::vec4 p0_ = model * v0;
    glm::vec4 p1_ = model * v1;
    glm::vec4 p2_ = model * v2;
    p0_ /= p0_.w;
    p1_ /= p1_.w;
    p2_ /= p2_.w;
    glm::vec3 p0 = glm::vec3(p0_);
    glm::vec3 p1 = glm::vec3(p1_);
    glm::vec3 p2 = glm::vec3(p2_);

    p0_ = vm * v0;
    p1_ = vm * v1;
    p2_ = vm * v2;
    glm::vec4 tp0 = p0_ / p0_.w;
    glm::vec4 tp1 = p1_ / p1_.w;
    glm::vec4 tp2 = p2_ / p2_.w;

    return {p0, p1, p2, tp0, tp1, tp2};
}

}  // namespace eng
