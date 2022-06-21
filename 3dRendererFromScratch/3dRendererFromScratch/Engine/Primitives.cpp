#include "Primitives.h"

namespace eng {

float segmentPlaneIntersect(glm::vec3 point, glm::vec3 normal, glm::vec3 v1, glm::vec3 v2) {
    normal = glm::normalize(normal);

    float dot = glm::dot(point, normal);
    float dot1 = glm::dot(v1, normal);
    float dot2 = glm::dot(v2, normal);

    return (dot - dot1) / (dot2 - dot1);
}

}  // namespace eng
