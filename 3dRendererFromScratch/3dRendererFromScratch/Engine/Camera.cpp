#include "Camera.h"

namespace eng {

Camera::Camera(glm::vec3 position, glm::vec3 direction) : position_(position), direction_(direction) {
}

const glm::mat4& Camera::getViewMatrix() const {
    return viewMatrix_;
}

const glm::vec3& Camera::getPosition() const {
    return position_;
}
const glm::vec3& Camera::getDirection() const {
    return direction_;
}

void Camera::setPosition(glm::vec3 position) {
    position_ = position;

    viewMatrix_ = glm::lookAt(position_, position_ + direction_, glm::vec3(0.0f, 1.0f, 0.0f));
}
void Camera::setDirection(glm::vec3 direction) {
    direction_ = direction;

    viewMatrix_ = glm::lookAt(position_, position_ + direction_, glm::vec3(0.0f, 1.0f, 0.0f));
}

}  // namespace eng
