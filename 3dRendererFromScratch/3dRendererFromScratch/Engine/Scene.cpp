#include "Scene.h"

namespace eng {

Scene::Scene()
    : objects_(std::pmr::vector<CubemapMesh>{Pool::getPool()}, std::pmr::vector<TextureMesh>{Pool::getPool()}, std::pmr::vector<FlatMesh>{Pool::getPool()},
              std::pmr::vector<UVMesh>{Pool::getPool()}, std::pmr::vector<NormalMesh>{Pool::getPool()}, std::pmr::vector<PhongMesh>{Pool::getPool()},
              std::pmr::vector<NormalMapMesh>{Pool::getPool()}){};

const Camera& Scene::getCamera() const { return camera_; }
Camera& Scene::getCamera() { return camera_; }

const LightsVec& Scene::getPointLights() const { return pointLights_; }
LightsVec& Scene::getPointLights() { return pointLights_; }

const ObjectsVec& Scene::getAllObjects() const { return objects_; }
ObjectsVec& Scene::getAllObjects() { return objects_; }

}  // namespace eng
