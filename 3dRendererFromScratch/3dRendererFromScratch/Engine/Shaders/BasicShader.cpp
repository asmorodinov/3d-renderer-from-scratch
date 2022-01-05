#include "BasicShader.h"

namespace eng {

void BasicVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    model = model_;
    view = view_;
    projection = projection_;

    viewPos = viewPos_;

    vm = view * model;

    normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
}

BasicVertexShader::BasicVertexShaderOutput BasicVertexShader::run(const WorldSpaceTriangle& tr) {
    glm::vec4 v0 = glm::vec4(tr.v0, 1.0f);
    glm::vec4 v1 = glm::vec4(tr.v1, 1.0f);
    glm::vec4 v2 = glm::vec4(tr.v2, 1.0f);

    glm::vec3 p0 = glm::vec3(model * v0);
    glm::vec3 p1 = glm::vec3(model * v1);
    glm::vec3 p2 = glm::vec3(model * v2);

    glm::vec4 tp0 = vm * v0;
    glm::vec4 tp1 = vm * v1;
    glm::vec4 tp2 = vm * v2;

    return {p0, p1, p2, tp0, tp1, tp2};
}

}  // namespace eng
