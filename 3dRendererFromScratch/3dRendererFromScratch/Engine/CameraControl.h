#pragma once
#include <cmath>
#include <algorithm>

#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace eng {

enum struct MovementDirection { Forward, Backward, Left, Right, Up, Down };

class CameraControl {
 public:
    CameraControl(glm::vec3 position = glm::vec3(0.0f, 0.0f, 6.0f), glm::vec3 direction = glm::vec3(0, 0, -1))
        : position(position), direction(direction), keys() {}
    void mouseMove(float dx, float dy);
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode);

    void update(float dt);

 public:
    glm::vec3 position;
    glm::vec3 direction;

 private:
    float yaw = -glm::pi<float>() / 2;
    float pitch = 0.0f;

    bool keys[6];
};

}  // namespace eng
