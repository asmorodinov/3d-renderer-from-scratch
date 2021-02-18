#pragma once

#include <glm/glm.hpp>

namespace eng {

glm::mat4 generateProjection(float fovy, float aspect, float zNear, float zFar);

}  // namespace eng
