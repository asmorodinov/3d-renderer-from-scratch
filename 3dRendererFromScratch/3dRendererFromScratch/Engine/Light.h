#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

struct PointLight {
    static constexpr const float defaultIntensity = 1.0f;
    static constexpr const float defaultSpecularCoefficient = 0.4f;
    static constexpr const float defaultDiffuseCoefficient = 1.2f;
    static constexpr const float defaultLinearAttenuationCoefficient = 0.0f;
    static constexpr const float defaultQuadraticAttenuationCoefficient = 0.0f;
    static constexpr const float defaultCubicAttenuationCoefficient = 0.00001f;

    PointLight(glm::vec3 position, glm::vec3 color, float intensity = defaultIntensity, float specularCoefficient = defaultSpecularCoefficient,
               float diffuseCoefficient = defaultDiffuseCoefficient, float linearAttenuationCoefficient = defaultLinearAttenuationCoefficient,
               float quadraticAttenuationCoefficient = defaultQuadraticAttenuationCoefficient,
               float cubicAttenuationCoefficient = defaultCubicAttenuationCoefficient);

    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float specularCoefficient;
    float diffuseCoefficient;

    float linearAttenuationCoefficient;
    float quadraticAttenuationCoefficient;
    float cubicAttenuationCoefficient;
};

using LightsVec = std::vector<PointLight>;

}  // namespace eng
