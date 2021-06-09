#include "Scene.h"

namespace eng {

Scene::Scene(){};

const Camera& Scene::getCamera() const {
    return camera_;
}

Camera& Scene::getCamera() {
    return camera_;
}

const ObjectsVec& Scene::getAllObjects() const {
    return objects_;
}

ObjectsVec& Scene::getAllObjects() {
    return objects_;
}

const LightsVec& Scene::getPointLights() const {
    return pointLights_;
}

LightsVec& Scene::getPointLights() {
    return pointLights_;
}

}  // namespace eng
