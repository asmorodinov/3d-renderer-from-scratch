#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

struct PointLight {
    PointLight(glm::vec3 pos, glm::vec3 color = glm::vec3(1.0f), float intensity = 1.0f, float spec = 0.4f, float diff = 1.2f, float lin = 0.0f,
               float quad = 0.0f, float cube = 0.00001f);

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
