#include "Scene.h"

namespace eng {

Scene::Scene(const std::string& name) : name_(name){};

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

const std::string& Scene::getName() const {
    return name_;
}
std::string& Scene::getName() {
    return name_;
}

const std::string& Scene::getPipeline() const {
    return pipeline_;
}
std::string& Scene::getPipeline() {
    return pipeline_;
}

}  // namespace eng
