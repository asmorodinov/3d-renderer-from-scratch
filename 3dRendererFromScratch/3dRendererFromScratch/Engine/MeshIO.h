#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <functional>

#include <fstream>
#include <cstdlib>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Assets.h"
#include "Scene.h"

#define READ_MEMBER(member, T)  \
    if (name == #member) {      \
        T val;                  \
        file >> val;            \
        res.##member## = val;   \
        correctProperty = true; \
    }
#define READ_VEC3_MEMBER(member)         \
    if (name == #member) {               \
        glm::vec3 val;                   \
        file >> val.x >> val.y >> val.z; \
        res.##member## = val;            \
        correctProperty = true;          \
    }

namespace eng {

struct Properties {
    std::string typeName = "FlatMesh";
    std::string diffuseTextureName = "brickwall.jpg";
    std::string normalTextureName = "brickwall_normal.jpg";
    std::string cubemapTextureName = "LancellottiChapel";
    std::string meshFileName = "light";
    std::string cubemapImageFormat = ".jpg";
    float meshScale = 1.0f;
    bool meshInvertNormals = false;
    bool meshOnlyVertices = true;
    bool wireframeMode = false;
    bool writeToDepthBuffer = true;
    bool cubemapDefaultFormat = true;
    glm::vec3 transformPosition = glm::vec3();
    glm::vec3 transformScale = glm::vec3(1.0f);
    glm::vec3 wireframeColor = glm::vec3(1.0f);
    glm::vec3 flatColor = glm::vec3(1.0f);
};

Properties loadProperties(std::ifstream& file);

CubemapTextureRef getCubemapTextureFromProperties(const Properties& pr);

// create mesh from properties
template <typename Mesh>
Mesh createMesh(const Properties& pr, std::function<typename Mesh::VertexShaderUniform(const Properties&)> f1,
                std::function<typename Mesh::FragmentShaderUniform(const Properties&)> f2) {
    auto meshData = Assets::getMeshData(pr.meshFileName, pr.meshScale, pr.meshInvertNormals, pr.meshOnlyVertices);
    auto transform = ObjectTransform(pr.transformPosition, pr.transformScale);

    return Mesh(meshData, f1(pr), f2(pr), transform, pr.wireframeMode, pr.writeToDepthBuffer, pr.wireframeColor);
}

FlatMesh createFlatMesh(const Properties& pr);

TextureMesh createTextureMesh(const Properties& pr);

CubemapMesh createCubemapMesh(const Properties& pr);
// todo: add more createMesh functions

void addMesh(const Properties& pr, Scene& scene);

Scene loadSceneFromFile(std::string fileName);

}  // namespace eng
