#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eng {

class ObjectTransform {
 public:
    ObjectTransform() = default;
    ObjectTransform(glm::vec3 pos);

    glm::mat4 getModel();

    glm::vec3 getPosition() const;
    glm::quat getOrientation() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3 pos);
    void setOrientation(const glm::quat orient);
    void setScale(const glm::vec3 sc);

 private:
    glm::vec3 position_ = glm::vec3(0.0f);
    glm::quat orientation_ = glm::quat();
    glm::vec3 scale_ = glm::vec3(1.0f);

    bool modelInvalid_ = true;
    glm::mat4 model_ = glm::mat4(1.0f);
};

}  // namespace eng
