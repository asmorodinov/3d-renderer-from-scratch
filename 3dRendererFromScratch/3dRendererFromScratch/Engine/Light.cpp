#include "Light.h"

namespace eng {

PointLight::PointLight(glm::vec3 position, glm::vec3 color, float intensity, float specularCoefficient, float diffuseCoefficient,
                       float linearAttenuationCoefficient, float quadraticAttenuationCoefficient, float cubicAttenuationCoefficient)
    : position(position),
      color(color),
      intensity(intensity),
      specularCoefficient(specularCoefficient),
      diffuseCoefficient(diffuseCoefficient),
      linearAttenuationCoefficient(linearAttenuationCoefficient),
      quadraticAttenuationCoefficient(quadraticAttenuationCoefficient),
      cubicAttenuationCoefficient(cubicAttenuationCoefficient) {
}

}  // namespace eng
