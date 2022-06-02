#include "FlatShader.h"

namespace eng {

Color32 FlatShader::computePixelColor(const Var& var, const LightsVec& lights) {
    return ClampConversion<Color128, Color32>::convertColor(Color128(uniform.flatColor, 1.0f));
}

void FlatVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

FlatVertexShader::Output FlatVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {}, {}, {}}, {}};
}

}  // namespace eng
