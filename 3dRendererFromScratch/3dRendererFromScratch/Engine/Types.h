#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Vector2d.h"

namespace eng {

using Byte = unsigned char;
using Pixels = size_t;
using AspectRatio = float;
using Degrees = float;
using Depth = float;
using Distance = float;

using Color128 = glm::vec4;   // float, 4 * 32 = 128 bits per color
using Color96 = glm::vec3;    // float, 3 * 32 = 96 bits per color
using Color32 = glm::u8vec4;  // u8, 4 * 8 = 32 bits per color

using ColorBuffer32 = Vector2d<Color32>;
using ColorBuffer96 = Vector2d<Color96>;
using ColorBuffer128 = Vector2d<Color128>;

}  // namespace eng
