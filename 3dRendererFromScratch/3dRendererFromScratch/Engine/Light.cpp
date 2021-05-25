#include "Light.h"

namespace eng {

PointLight::PointLight(glm::vec3 pos, glm::vec3 color, float intensity, float spec, float diff, float lin, float quad, float cube)
    : position(pos), color(color), intensity(intensity), specularCoefficient(spec), diffuseCoefficient(diff), linearAttenuationCoefficient(lin), quadraticAttenuationCoefficient(quad), cubicAttenuationCoefficient(cube) {}

}  // namespace eng
