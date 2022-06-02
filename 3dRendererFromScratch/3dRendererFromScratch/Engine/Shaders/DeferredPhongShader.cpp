#include "DeferredPhongShader.h"

namespace eng {

namespace PhongShaderCommon {

glm::vec3 computePhong(glm::vec3 viewPos, const GeometryInfo& info, const LightsVec& lights) {
    auto FragPos = info.position;
    auto normal = info.normal;
    auto color = info.diffuseColor;

    auto lighting = glm::vec3(0.0f);
    auto viewDir = glm::normalize(viewPos - FragPos);

    for (const auto& light : lights) {
        auto lightDir = glm::normalize(light.position - FragPos);

        auto diff = glm::max(glm::dot(normal, lightDir), 0.0f);
        auto diffuse = diff * color * light.color * light.diffuseCoefficient;

        auto halfwayDir = glm::normalize(lightDir + viewDir);
        auto spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
        auto specular = light.color * spec * light.specularCoefficient;

        auto d = glm::length(light.position - FragPos);
        auto attenuation = 1.0f / (1.0f + d * 0.1f + d * d * 0.01f);
        diffuse *= attenuation;
        specular *= attenuation;
        lighting += light.intensity * (diffuse + specular);
    }

    return lighting;
}

}  // namespace PhongShaderCommon

namespace DeferredPhongShader {

Color128 computePixelColor(glm::vec3 viewPos, const GeometryInfo& info, const LightsVec& lights) {
    auto lighting = 0.3f * info.diffuseColor;
    lighting += PhongShaderCommon::computePhong(viewPos, info, lights);

    return Color128(lighting, 1.0f);
}

}  // namespace DeferredPhongShader

namespace SSAOPhongShader {

Color128 computePixelColor(glm::vec3 viewPos, float occlusion, const GeometryInfo& info, const LightsVec& lights) {
    auto lighting = 0.3f * info.diffuseColor * occlusion;
    lighting += PhongShaderCommon::computePhong(viewPos, info, lights);

    return Color128(lighting, 1.0f);
}

}  // namespace SSAOPhongShader

}  // namespace eng
