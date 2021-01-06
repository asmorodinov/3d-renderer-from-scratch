#pragma once
#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

enum struct MovementDirection { Forward, Backward, Left, Right, Up, Down };

class PlayerCamera {
 public:
    PlayerCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3 direction = glm::vec3(0, 0, -1))
        : position(position), direction(direction), keys() {}
    void mouseMove(float dx, float dy) {
        constexpr static float sensitivity = 0.2f;
        float xoffset = sensitivity * dx;
        float yoffset = -sensitivity * dy;

        yaw += glm::radians(xoffset);
        pitch += glm::radians(yoffset);

        pitch = std::max(glm::radians(-89.0f), std::min(glm::radians(89.0f), pitch));

        direction.x = std::cos(yaw) * std::cos(pitch);
        direction.y = std::sin(pitch);
        direction.z = std::sin(yaw) * std::cos(pitch);
        direction = glm::normalize(direction);
    }
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode) {
        switch (key) {
            case sf::Keyboard::A:
                keys[static_cast<int>(MovementDirection::Left)] = mode;
                break;
            case sf::Keyboard::C:
                keys[static_cast<int>(MovementDirection::Down)] = mode;
                break;
            case sf::Keyboard::D:
                keys[static_cast<int>(MovementDirection::Right)] = mode;
                break;
            case sf::Keyboard::S:
                keys[static_cast<int>(MovementDirection::Backward)] = mode;
                break;
            case sf::Keyboard::V:
                keys[static_cast<int>(MovementDirection::Up)] = mode;
                break;
            case sf::Keyboard::W:
                keys[static_cast<int>(MovementDirection::Forward)] = mode;
                break;
            default:
                break;
        }
    }

    void update(float dt) {
        float speed = 1.0f;

        glm::vec3 front = glm::normalize(glm::vec3(direction.x, 0, direction.z));
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(front, up));

        glm::vec3 movement = glm::vec3(0.0f);

        if (keys[static_cast<int>(MovementDirection::Forward)]) movement += front;
        if (keys[static_cast<int>(MovementDirection::Backward)]) movement -= front;
        if (keys[static_cast<int>(MovementDirection::Left)]) movement -= right;
        if (keys[static_cast<int>(MovementDirection::Right)]) movement += right;
        if (keys[static_cast<int>(MovementDirection::Up)]) movement += up;
        if (keys[static_cast<int>(MovementDirection::Down)]) movement -= up;

        if (movement != glm::vec3(0.0f)) movement = glm::normalize(movement);

        position += speed * dt * movement;
    }

 public:
    glm::vec3 position;
    glm::vec3 direction;

 private:
    float yaw = -glm::pi<float>() / 2;
    float pitch = 0.0f;

    bool keys[6];
};

}  // namespace eng
