#include "World.h"

namespace eng {

const Camera& World::getCamera() const { return camera; }
Camera& World::getCamera() { return camera; }
const std::vector<ObjectPtr>& World::getObjects() const { return objects; }
std::vector<ObjectPtr>& World::getObjects() { return objects; }

}  // namespace eng
