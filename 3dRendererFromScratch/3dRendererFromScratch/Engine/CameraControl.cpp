#include "CameraControl.h"

namespace eng {

CameraControl::CameraControl(glm::vec3 position, glm::vec3 direction) : position_(position), direction_(direction), keys_() {}

void CameraControl::mouseMove(glm::vec2 delta) {
    constexpr static float sensitivity = 0.2f;
    glm::vec2 offset = sensitivity * delta;
    offset.y *= -1.0f;

    yaw_ += glm::radians(offset.x);
    pitch_ += glm::radians(offset.y);

    pitch_ = std::max(glm::radians(-89.0f), std::min(glm::radians(89.0f), pitch_));

    direction_.x = std::cos(yaw_) * std::cos(pitch_);
    direction_.y = std::sin(pitch_);
    direction_.z = std::sin(yaw_) * std::cos(pitch_);
    direction_ = glm::normalize(direction_);
}
void CameraControl::keyPressedOrReleased(sf::Keyboard::Key key, bool mode) {
    switch (key) {
        case sf::Keyboard::A:
            keys_[static_cast<int>(MovementDirection::Left)] = mode;
            break;
        case sf::Keyboard::C:
            keys_[static_cast<int>(MovementDirection::Down)] = mode;
            break;
        case sf::Keyboard::D:
            keys_[static_cast<int>(MovementDirection::Right)] = mode;
            break;
        case sf::Keyboard::S:
            keys_[static_cast<int>(MovementDirection::Backward)] = mode;
            break;
        case sf::Keyboard::V:
            keys_[static_cast<int>(MovementDirection::Up)] = mode;
            break;
        case sf::Keyboard::W:
            keys_[static_cast<int>(MovementDirection::Forward)] = mode;
            break;
        default:
            break;
    }
}
void CameraControl::update(float dt) {
    float speed = 1.0f;

    glm::vec3 front = glm::normalize(glm::vec3(direction_.x, 0, direction_.z));
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    glm::vec3 movement = glm::vec3(0.0f);

    if (keys_[static_cast<int>(MovementDirection::Forward)]) movement += front;
    if (keys_[static_cast<int>(MovementDirection::Backward)]) movement -= front;
    if (keys_[static_cast<int>(MovementDirection::Left)]) movement -= right;
    if (keys_[static_cast<int>(MovementDirection::Right)]) movement += right;
    if (keys_[static_cast<int>(MovementDirection::Up)]) movement += up;
    if (keys_[static_cast<int>(MovementDirection::Down)]) movement -= up;

    if (movement != glm::vec3(0.0f)) movement = glm::normalize(movement);

    position_ += speed * dt * movement;
}

void CameraControl::setPosition(glm::vec3 pos) { position_ = pos; }
glm::vec3 CameraControl::getPosition() const { return position_; }

void CameraControl::setDirection(glm::vec3 dir) { direction_ = dir; }
glm::vec3 CameraControl::getDirection() const { return direction_; }

}  // namespace eng
