#pragma once

#include "BasicShader.h"
#include "GeometryPassShader.h"

namespace eng {
namespace DeferredPhongShader {

Color128 computePixelColor(glm::vec3 viewPos, const GeometryInfo& info, const LightsVec& lights);

}
}  // namespace eng
