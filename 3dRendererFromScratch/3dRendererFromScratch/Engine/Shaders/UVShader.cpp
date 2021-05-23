#include "UVShader.h"

namespace eng {

glm::vec4 UVShader::computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(var.t.x, var.t.y, 0.5f, 1.0f); }

}  // namespace eng
