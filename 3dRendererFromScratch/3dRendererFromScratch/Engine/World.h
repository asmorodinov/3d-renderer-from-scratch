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

    const ObjectsVec& getObjects() const;
    ObjectsVec& getObjects();

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

 private:
    Camera camera;
    ObjectsVec objects;
    LightsVec pointLights;
};

}  // namespace eng
