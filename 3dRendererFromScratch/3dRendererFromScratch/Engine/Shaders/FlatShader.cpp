#include "FlatShader.h"

namespace eng {

void FlatVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

glm::vec4 FlatShader::computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(uniform, 1.0f); }

FlatVertexShader::Output FlatVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {}, {}, {}}, {}};
}

}  // namespace eng
