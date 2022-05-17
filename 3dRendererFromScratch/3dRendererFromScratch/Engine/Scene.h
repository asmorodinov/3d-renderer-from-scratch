#pragma once

#include <string>
#include <tuple>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shaders.h"
#include "MeshTypes.h"
#include "Camera.h"

namespace eng {

class Scene {
 public:
    Scene() = default;
    Scene(const std::string& name);

    const Camera& getCamera() const;
    Camera& getCamera();

    const ObjectsVec& getAllObjects() const;
    ObjectsVec& getAllObjects();

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

    const std::string& getName() const;
    std::string& getName();

    const std::string& getPipeline() const;
    std::string& getPipeline();

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

    std::string name_ = "unnamed scene";

    std::string pipeline_ = "default";
};

}  // namespace eng
