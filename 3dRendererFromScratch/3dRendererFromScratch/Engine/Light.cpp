#include "Light.h"

namespace eng {

PointLight::PointLight(glm::vec3 pos, glm::vec3 color, float intensity, float spec, float diff, float lin, float quad, float cube)
    : pos(pos), color(color), intensity(intensity), spec(spec), diff(diff), lin(lin), quad(quad), cube(cube) {}

}  // namespace eng
