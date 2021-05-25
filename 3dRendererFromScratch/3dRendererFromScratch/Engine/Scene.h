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

    template <typename T>
    const std::pmr::vector<T>& getObjects() const {
        return std::get<std::pmr::vector<T>>(objects_);
    }
    template <typename T>
    std::pmr::vector<T>& getObjects() {
        return std::get<std::pmr::vector<T>>(objects_);
    }

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

    template <typename T>
    void addObject(const T& t) {
        std::get<std::pmr::vector<T>>(objects_).push_back(t);
    }

 private:
    Camera camera_;
    ObjectsVec objects_;
    LightsVec pointLights_;
};

}  // namespace eng
