#include "Scene.h"

namespace eng {

Scene::Scene()
    : objects(std::pmr::vector<CubemapMesh>{Pool::getPool()}, std::pmr::vector<TextureMesh>{Pool::getPool()}, std::pmr::vector<FlatMesh>{Pool::getPool()},
              std::pmr::vector<UVMesh>{Pool::getPool()}, std::pmr::vector<NormalMesh>{Pool::getPool()}, std::pmr::vector<PhongMesh>{Pool::getPool()},
              std::pmr::vector<NormalMapMesh>{Pool::getPool()}){};

const Camera& Scene::getCamera() const { return camera; }
Camera& Scene::getCamera() { return camera; }

const LightsVec& Scene::getPointLights() const { return pointLights; }
LightsVec& Scene::getPointLights() { return pointLights; }

const ObjectsVec& Scene::getAllObjects() const { return objects; }
ObjectsVec& Scene::getAllObjects() { return objects; }

}  // namespace eng
