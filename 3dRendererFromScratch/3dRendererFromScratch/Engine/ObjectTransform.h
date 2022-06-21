#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eng {

// stores transformation of 3d object: translation, scaling and orientation (rotation)
// https://www.tutorialspoint.com/computer_graphics/3d_transformation.htm

class ObjectTransform {
 public:
    ObjectTransform() = default;
    ObjectTransform(glm::vec3 pos);
    ObjectTransform(glm::vec3 pos, glm::vec3 scale);

    const glm::mat4& getModel();

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3 position);
    void setOrientation(const glm::quat orientation);
    void setScale(const glm::vec3 scale);

 private:
    glm::vec3 position_ = glm::vec3(0.0f);
    glm::quat orientation_ = glm::quat();
    glm::vec3 scale_ = glm::vec3(1.0f);

    bool modelInvalid_ = true;
    glm::mat4 model_ = glm::mat4(1.0f);
};

}  // namespace eng
