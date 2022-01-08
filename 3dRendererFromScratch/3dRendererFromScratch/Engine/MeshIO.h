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

#define ADD_MESH(meshType)                                      \
    if (typeName == #meshType) {                                \
        scene.addObject(pr.name, createMesh<##meshType##>(pr)); \
        correctMeshType = true;                                 \
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

PointLight loadPointLightFromFile(std::ifstream& file);

CubemapTextureRef getCubemapTextureFromProperties(const Properties& pr);

// create mesh from properties
template <typename Mesh>
Mesh createMesh(const Properties& pr) {
    auto meshData = Assets::getMeshData(pr.meshFileName, pr.meshScale, pr.meshInvertNormals, pr.meshOnlyVertices);
    auto transform = ObjectTransform(pr.transformPosition, pr.transformScale);

    VertexShaderUniform vun = {};

    FragmentShaderUniform fun = {
        Assets::getTexture(pr.diffuseTextureName),  // diffuse
        Assets::getTexture(pr.normalTextureName),   // normmal
        getCubemapTextureFromProperties(pr),        // cubemap
        pr.flatColor                                // flat color
    };

    return Mesh(meshData, vun, fun, transform, pr.wireframeMode, pr.writeToDepthBuffer, pr.wireframeColor, pr.drawingEnabled);
}

// get properties from Mesh
template <typename Mesh>
Properties getProperties(const Mesh& mesh, std::string name) {
    auto meshData = mesh.getMeshData();
    const auto& transform = mesh.getTransform();

    Properties pr;
    pr.name = name;
    pr.typeName = TypeNameMap<Mesh>::name;

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

    // fragment shader uniform properties
    const FragmentShaderUniform& uniform = mesh.getFragmentShaderUniform();
    pr.diffuseTextureName = uniform.diffuseTexture.get().fileName;
    pr.flatColor = uniform.flatColor;
    pr.cubemapTextureName = uniform.cubemapTexture.get().fileName;
    pr.cubemapDefaultFormat = uniform.cubemapTexture.get().defaultFormat;
    pr.cubemapImageFormat = uniform.cubemapTexture.get().imageFormat;
    pr.normalTextureName = uniform.normalTexture.get().fileName;

    return pr;
}

void addMesh(const Properties& pr, Scene& scene);

Scene loadSceneFromFile(std::string fileName);
bool saveSceneToFile(const Scene& scene, std::string fileName);

// get properties
Properties getMeshProperties(const MeshVariant& mesh, std::string name);

}  // namespace eng
