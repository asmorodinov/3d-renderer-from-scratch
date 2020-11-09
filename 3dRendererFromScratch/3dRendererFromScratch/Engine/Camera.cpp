#include "Camera.h"

namespace eng {

Camera::Camera(glm::vec3 position, glm::vec3 direction)
    : position(position), direction(direction) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

void Camera::setPosition(glm::vec3 pos) { position = pos; }
glm::vec3 Camera::getPosition() const { return position; }

void Camera::setDirection(glm::vec3 dir) { direction = dir; }
glm::vec3 Camera::getDirection() const { return direction; }

}  // namespace eng
