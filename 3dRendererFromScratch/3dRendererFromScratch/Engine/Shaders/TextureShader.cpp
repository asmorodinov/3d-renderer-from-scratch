#include "TextureShader.h"

namespace eng {

glm::vec4 TextureShader::computePixelColor(const Var& var, const LightsVec& lights) {
    return uniform.diffuseTexture.get().sample(var.t);
}

void TextureVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

TextureVertexShader::Output TextureVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, tr.t0, tr.t1, tr.t2}, {}};
}

}  // namespace eng
