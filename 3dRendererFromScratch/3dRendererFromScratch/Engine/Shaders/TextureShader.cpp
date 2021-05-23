#include "TextureShader.h"

namespace eng {

void TextureVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

glm::vec4 TextureShader::computePixelColor(const Var& var, const LightsVec& lights) { return uniform.get().sample(var.t.x, var.t.y); }

TextureVertexShader::Output TextureVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, tr.t0, tr.t1, tr.t2}, {}};
}

}  // namespace eng
