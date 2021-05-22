#include "Scene.h"

namespace eng {

const Camera& Scene::getCamera() const { return camera; }
Camera& Scene::getCamera() { return camera; }

const ObjectsVec& Scene::getObjects() const { return objects; }
ObjectsVec& Scene::getObjects() { return objects; }

const LightsVec& Scene::getPointLights() const { return pointLights; }
LightsVec& Scene::getPointLights() { return pointLights; }

}  // namespace eng
