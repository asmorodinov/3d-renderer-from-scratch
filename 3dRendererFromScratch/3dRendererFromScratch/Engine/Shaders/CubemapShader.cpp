#include "CubemapShader.h"

namespace eng {

void CubemapVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
    bvs.view = glm::mat4(glm::mat3(bvs.view));
    bvs.vm = bvs.view * bvs.model;
}

CubemapVertexShader::Output CubemapVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0}, {bvso.v1}, {bvso.v2}}, {}};
}

glm::vec4 CubemapShader::computePixelColor(const Var& var, const LightsVec& lights) { return uniform.get().sample(var.t); }

}  // namespace eng
