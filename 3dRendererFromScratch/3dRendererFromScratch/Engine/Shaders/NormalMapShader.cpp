#include "NormalMapShader.h"

namespace eng {

glm::vec4 NormalMapShader::computePixelColor(const Var& var, const LightsVec& lights) {
    auto FragPos = var.t1;
    auto uv = var.t2;

    auto diffuseMap = uniform.diffuseMap;
    auto normalMap = uniform.normalMap;
    auto viewPos = vso;
    auto normal = glm::normalize(glm::vec3(normalMap.get().sample(uv)) * 2.0f - 1.0f);

    glm::vec4 color = diffuseMap.get().sample(uv);

    glm::vec3 lighting = glm::vec3(0.0f);

    for (const auto& light : lights) {
        glm::vec3 lightPos = light.position;
        glm::vec3 viewDir = glm::normalize(viewPos - FragPos);
        glm::vec3 lightDir = glm::normalize(lightPos - FragPos);

        float diff = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.2f);
        glm::vec3 diffuse = diff * glm::vec3(color) * light.color * light.diffuseCoefficient;

        glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
        float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
        glm::vec3 specular = light.color * spec * light.specularCoefficient;

        float d = length(lightPos - FragPos);
        float attenuation = 1.0f / (1.0f + d * light.linearAttenuationCoefficient + std::pow(d, 2.0f) * light.quadraticAttenuationCoefficient +
                                    std::pow(d, 3.0f) * light.cubicAttenuationCoefficient);
        diffuse *= attenuation / 1.7;
        specular *= attenuation / 1.7;
        lighting += 1.0f * light.intensity * glm::vec3(1.0f) * (diffuse + specular);
    }

    lighting = glm::pow(lighting, glm::vec3(1.0f / 2.2f));

    return glm::vec4(lighting, 1.0f);
}

void NormalMapVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

NormalMapVertexShader::Output NormalMapVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);
    return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0, tr.t0}, {bvso.v1, tr.t1}, {bvso.v2, tr.t2}}, bvs.viewPos};
}

}  // namespace eng
