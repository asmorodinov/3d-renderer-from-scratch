#pragma once

#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VertexShader.h"
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
        return std::get<std::pmr::vector<T>>(objects);
    }
    template <typename T>
    std::pmr::vector<T>& getObjects() {
        return std::get<std::pmr::vector<T>>(objects);
    }

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

    template <typename T>
    void addObject(const T& t) {
        std::get<std::pmr::vector<T>>(objects).push_back(t);
    }

 private:
    Camera camera;
    ObjectsVec objects;
    LightsVec pointLights;
};

}  // namespace eng
