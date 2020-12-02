#include "World.h"

namespace eng {

const Camera& World::getCamera() const { return camera; }
Camera& World::getCamera() { return camera; }

const ObjectsVec& World::getObjects() const { return objects; }
ObjectsVec& World::getObjects() { return objects; }

const LightsVec& World::getPointLights() const { return pointLights; }
LightsVec& World::getPointLights() { return pointLights; }

}  // namespace eng
