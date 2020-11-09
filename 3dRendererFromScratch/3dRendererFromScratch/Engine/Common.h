#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "Screen.h"

namespace eng {

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat orientation = glm::quat();
    glm::vec3 scale = glm::vec3(1.0f);

    // This is a temporary hack, will be removed later   [TODO]: Remove
    glm::mat4 extraTransform = glm::mat4(1.0f);

    glm::mat4 getModel() const;
};

glm::vec2 LocalCoordsToScreenCoords(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
                                    const glm::mat4& objectModel, glm::vec3 localCoords);

}  // namespace eng
