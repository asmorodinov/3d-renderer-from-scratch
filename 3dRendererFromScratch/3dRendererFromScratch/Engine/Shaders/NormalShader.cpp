#include "NormalShader.h"

namespace eng {

void NormalVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

glm::vec4 NormalShader::computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(0.5f * (vso + 1.0f), 1.0f); }

NormalVertexShader::Output NormalVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {}, {}, {}}, {bvs.normalMatrix * tr.n0}};
}

}  // namespace eng
