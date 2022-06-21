#include "GeometryPassShader.h"

namespace eng {

// fragment shader

GeometryInfo GeometryPassShader::computePixelColor(const Var& var, const LightsVec& lights) {
    auto FragPos = var.t1;
    auto uv = var.t2;
    auto normal = vso.normal;
    auto color = glm::vec3(uniform.diffuseTexture.get().sample(uv));

    return GeometryInfo{FragPos, normal, color};
}

// deferred shading

void GeometryPassVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

GeometryPassVertexShader::Output GeometryPassVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0, tr.t0}, {bvso.v1, tr.t1}, {bvso.v2, tr.t2}}, {glm::normalize(bvs.normalMatrix * tr.normal)}};
}

// SSAO

void SSAOGeometryPassVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(view_ * model_)));
}

SSAOGeometryPassVertexShader::Output SSAOGeometryPassVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {glm::vec3(bvso.cv0), tr.t0}, {glm::vec3(bvso.cv1), tr.t1}, {glm::vec3(bvso.cv2), tr.t2}},
            {glm::normalize(normalMatrix * tr.normal)}};
}

}  // namespace eng
