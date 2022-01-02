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

#define WRITE_MEMBER(member)                                              \
    if (properties.##member## != defaultProperties.##member##) {          \
        file << '\t' << #member << '\t' << properties.##member## << '\n'; \
    }

#define WRITE_VEC3_MEMBER(member)                                                                                                               \
    if (properties.##member## != defaultProperties.##member##) {                                                                                \
        file << '\t' << #member << "\t" << properties.##member##.x << ' ' << properties.##member##.y << ' ' << properties.##member##.z << '\n'; \
    }

#define READ_VEC3_MEMBER(member)         \
    if (name == #member) {               \
        glm::vec3 val;                   \
        file >> val.x >> val.y >> val.z; \
        res.##member## = val;            \
        correctProperty = true;          \
    }

#define ADD_MESH(meshType)                                \
    if (typeName == #meshType) {                          \
        scene.addObject(pr.name, create##meshType##(pr)); \
        correctMeshType = true;                           \
    }

namespace eng {

struct Properties {
    std::string name = "Mesh";
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
    bool drawingEnabled = true;
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

    return Mesh(meshData, f1(pr), f2(pr), transform, pr.wireframeMode, pr.writeToDepthBuffer, pr.wireframeColor, pr.drawingEnabled);
}

// get properties from Mesh
template <typename Mesh>
Properties getProperties(const Mesh& mesh, std::function<void(const typename Mesh::VertexShaderUniform&, Properties&)> f1,
                         std::function<void(const typename Mesh::FragmentShaderUniform&, Properties&)> f2, std::string typeName, std::string name) {
    auto meshData = mesh.getMeshData();
    const auto& transform = mesh.getTransform();

    Properties pr;
    pr.name = name;
    pr.typeName = typeName;
    pr.meshFileName = meshData.get().fileName;
    pr.meshInvertNormals = meshData.get().invertNormals;
    pr.meshOnlyVertices = meshData.get().onlyVertices;
    pr.meshScale = meshData.get().scale;
    pr.transformPosition = transform.getPosition();
    pr.transformScale = transform.getScale();
    pr.wireframeColor = mesh.getWireframeColor();
    pr.wireframeMode = mesh.getWireframeMode();
    pr.writeToDepthBuffer = mesh.getWriteToDepthBuffer();
    pr.drawingEnabled = mesh.getDrawingEnabled();

    f1(mesh.getVertexShaderUniform(), pr);
    f2(mesh.getFragmentShaderUniform(), pr);

    return pr;
}

FlatMesh createFlatMesh(const Properties& pr);
TextureMesh createTextureMesh(const Properties& pr);
CubemapMesh createCubemapMesh(const Properties& pr);
UVMesh createUVMesh(const Properties& pr);
NormalMesh createNormalMesh(const Properties& pr);
PhongMesh createPhongMesh(const Properties& pr);
NormalMapMesh createNormalMapMesh(const Properties& pr);

void addMesh(const Properties& pr, Scene& scene);

Scene loadSceneFromFile(std::string fileName);
bool saveSceneToFile(const Scene& scene, std::string fileName);

// get properties
Properties getMeshProperties(const FlatMesh& mesh, std::string name);
Properties getMeshProperties(const TextureMesh& mesh, std::string name);
Properties getMeshProperties(const CubemapMesh& mesh, std::string name);
Properties getMeshProperties(const UVMesh& mesh, std::string name);
Properties getMeshProperties(const NormalMesh& mesh, std::string name);
Properties getMeshProperties(const PhongMesh& mesh, std::string name);
Properties getMeshProperties(const NormalMapMesh& mesh, std::string name);

Properties getMeshProperties(const MeshVariant& mesh, std::string name);

}  // namespace eng
