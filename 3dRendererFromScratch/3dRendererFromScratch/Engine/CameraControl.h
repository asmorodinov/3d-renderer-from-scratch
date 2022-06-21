#pragma once

#include <array>
#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

namespace cameraControlDefaults {

static const glm::vec3 position = glm::vec3(0.0f, 0.0f, 6.0f);
static const glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);

}  // namespace cameraControlDefaults

class CameraControl {
 private:
    enum struct MovementDirection { Forward, Backward, Left, Right, Up, Down };

 public:
    using Seconds = float;

    CameraControl(glm::vec3 position = cameraControlDefaults::position, glm::vec3 direction = cameraControlDefaults::direction);

    void mouseMove(glm::vec2 delta);
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode);

    void update(Seconds deltaTime);

    glm::vec3 getPosition() const;
    glm::vec3 getDirection() const;

    void setPosition(glm::vec3 pos);
    void setDirection(glm::vec3 dir);

 private:
    glm::vec3 position_;
    glm::vec3 direction_;

    float yaw_ = -glm::pi<float>() / 2;
    float pitch_ = 0.0f;

    std::array<bool, 6> keys_ = {};
};

}  // namespace eng
