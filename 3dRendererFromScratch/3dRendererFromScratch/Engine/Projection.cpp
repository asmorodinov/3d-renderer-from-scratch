#include "Projection.h"

#include <glm/gtc/matrix_transform.hpp>

namespace eng {

glm::mat4 generateProjection(float fovy, float aspect, float zNear, float zFar) {
    assert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

    const float tanHalfFovy = std::tan(fovy / 2.0f);
    float h = 2.0f * zNear * tanHalfFovy;
    float w = aspect * h;

    return glm::frustum(-w / 2, w / 2, -h / 2, h / 2, zNear, zFar);
}

}  // namespace eng
