#include "PhongShadowMappingShader.h"

#include "../Texture.h"
#include "../Assets.h"

#include <iostream>

namespace eng {

float LinearizeDepth(float depth, float near_plane, float far_plane) {
    float z = depth * 2.0 - 1.0;  // Back to NDC
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

glm::vec4 PhongShadowMappingShader::computePixelColor(const Var& var, const LightsVec& lights) {
    auto FragPosLightSpace = var.t1;
    auto FragPos = var.t2;
    auto uv = var.t3;

    // calculate shadow

    const auto& shadowMappingInfo = Assets::getShadowMappingInfo();
    auto shadow = 0.0f;
    if (shadowMappingInfo.has_value()) {
        const auto& info = shadowMappingInfo.value();
        const auto& depthMap = info.depthMap.get();

        auto projCoords = glm::vec3(FragPosLightSpace / FragPosLightSpace.w);
        projCoords = projCoords * 0.5f + 0.5f;

        auto closestDepth = std::numeric_limits<float>::max();
        if (projCoords.x >= 0.0f && projCoords.x <= 1.0f && projCoords.y >= 0.0f && projCoords.y <= 1.0f) {
            projCoords.y = 1.0f - projCoords.y;
            closestDepth = sample2dBuffer(depthMap, glm::vec2(projCoords));
        }

        auto bias = 0.2f;
        auto currentDepth = projCoords.z;
        shadow = ((currentDepth - bias) > closestDepth) ? 1.0f : 0.0f;
    }

    // phong lighting

    auto texture = uniform.diffuseTexture;
    auto viewPos = vso.viewPos;
    auto normal = vso.normal;

    glm::vec4 color = texture.get().sample(uv);

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
        lighting += light.intensity * glm::vec3(1.0f) * (diffuse + specular);
    }

    lighting *= (1.0f - shadow * 0.8f);

    return glm::vec4(lighting, 1.0f);
}

void PhongShadowMappingVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

PhongShadowMappingVertexShader::Output PhongShadowMappingVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);

    auto fpls0 = glm::vec4(0.0f);
    auto fpls1 = glm::vec4(0.0f);
    auto fpls2 = glm::vec4(0.0f);
    const auto& shadowMappingInfo = Assets::getShadowMappingInfo();
    if (shadowMappingInfo.has_value()) {
        const auto& lightSpaceMatrix = shadowMappingInfo.value().lightSpaceMatrix.get();
        fpls0 = lightSpaceMatrix * glm::vec4(bvso.v0, 1.0f);
        fpls1 = lightSpaceMatrix * glm::vec4(bvso.v1, 1.0f);
        fpls2 = lightSpaceMatrix * glm::vec4(bvso.v2, 1.0f);
    }

    return {{bvso.cv0, bvso.cv1, bvso.cv2, {fpls0, bvso.v0, tr.t0}, {fpls1, bvso.v1, tr.t1}, {fpls2, bvso.v2, tr.t2}},
            {bvs.viewPos, glm::normalize(bvs.normalMatrix * tr.normal)}};
}

}  // namespace eng
