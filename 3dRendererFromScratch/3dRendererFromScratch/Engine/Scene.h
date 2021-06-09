#pragma once

#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shaders.h"
#include "MeshTypes.h"
#include "Camera.h"

namespace eng {

class Scene {
 public:
    Scene();

    const Camera& getCamera() const;
    Camera& getCamera();

    const ObjectsVec& getAllObjects() const;
    ObjectsVec& getAllObjects();

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

    template <typename T>
    void addObject(std::string name, const T& t) {
        if (objects_.count(name) == 0)
            objects_.insert({name, {t}});
        else
            objects_.at(name) = {t};
    }

 private:
    Camera camera_;
    ObjectsVec objects_;
    LightsVec pointLights_;
};

}  // namespace eng
