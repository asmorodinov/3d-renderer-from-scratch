#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

class Camera {
 public:
    Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 direction = glm::vec3(0, 0, -1));

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;

    void setPosition(glm::vec3 pos);
    void setDirection(glm::vec3 dir);

 private:
    glm::vec3 position_;
    glm::vec3 direction_;
};

}  // namespace eng
