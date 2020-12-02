#pragma once

#include <assert.h>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Primitives.h"
#include "Texture.h"
#include "Light.h"
#include "Camera.h"
#include "Screen.h"

namespace eng {

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat orientation = glm::quat();
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModel() const;
};

}  // namespace eng
