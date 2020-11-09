#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Object.h"
#include "Camera.h"

namespace eng {

class World {
 public:
    World(){};

    const Camera& getCamera() const;
    Camera& getCamera();
    const std::vector<ObjectPtr>& getObjects() const;
    std::vector<ObjectPtr>& getObjects();

 private:
    Camera camera;
    std::vector<ObjectPtr> objects;
};

}  // namespace eng
