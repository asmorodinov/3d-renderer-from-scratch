#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

namespace cameraDefaults {

static const glm::vec3 position = glm::vec3(0.0f);
static const glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

}  // namespace cameraDefaults

class Camera {
 public:
    Camera(glm::vec3 position = cameraDefaults::position, glm::vec3 direction = cameraDefaults::direction);

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;

    void setPosition(glm::vec3 position);
    void setDirection(glm::vec3 direction);

 private:
    glm::vec3 position_;
    glm::vec3 direction_;
};

}  // namespace eng
