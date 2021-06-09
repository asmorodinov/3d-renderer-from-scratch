#include "MeshIO.h"

namespace eng {

Properties loadProperties(std::ifstream& file) {
    Properties res;

    bool start = false;

    while (1) {
        std::string name;
        file >> name;

        if (file.fail()) break;

        if (name == "{") start = true;

        if (!start) continue;

        if (name == "}") break;

        bool correctProperty = false;

        READ_MEMBER(typeName, std::string)
        READ_MEMBER(diffuseTextureName, std::string)
        READ_MEMBER(normalTextureName, std::string)
        READ_MEMBER(cubemapTextureName, std::string)
        READ_MEMBER(meshFileName, std::string)
        READ_MEMBER(cubemapImageFormat, std::string)
        READ_MEMBER(meshScale, float)
        READ_MEMBER(meshInvertNormals, bool)
        READ_MEMBER(meshOnlyVertices, bool)
        READ_MEMBER(wireframeMode, bool)
        READ_MEMBER(writeToDepthBuffer, bool)
        READ_MEMBER(cubemapDefaultFormat, bool)
        READ_VEC3_MEMBER(transformPosition)
        READ_VEC3_MEMBER(transformScale)
        READ_VEC3_MEMBER(wireframeColor)
        READ_VEC3_MEMBER(flatColor)

        assert(correctProperty);
    }

    return res;
}

CubemapTextureRef getCubemapTextureFromProperties(const Properties& pr) {
    return Assets::getCubemapTexture(pr.cubemapTextureName, pr.cubemapDefaultFormat, pr.cubemapImageFormat);
}

FlatMesh createFlatMesh(const Properties& pr) {
    return createMesh<FlatMesh>(
        pr, [](const Properties& pr) -> FlatMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> FlatMesh::FragmentShaderUniform { return pr.flatColor; });
}

TextureMesh createTextureMesh(const Properties& pr) {
    return createMesh<TextureMesh>(
        pr, [](const Properties& pr) -> TextureMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> TextureMesh::FragmentShaderUniform { return Assets::getTexture(pr.diffuseTextureName); });
}

CubemapMesh createCubemapMesh(const Properties& pr) {
    return createMesh<CubemapMesh>(
        pr, [](const Properties& pr) -> CubemapMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> CubemapMesh::FragmentShaderUniform { return getCubemapTextureFromProperties(pr); });
}

void addMesh(const Properties& pr, Scene& scene) {
    std::string typeName = pr.typeName;

    if (typeName == "FlatMesh") {
        scene.addObject(createFlatMesh(pr));
    } else if (typeName == "TextureMesh") {
        scene.addObject(createTextureMesh(pr));
    } else if (typeName == "CubemapMesh") {
        scene.addObject(createCubemapMesh(pr));
    }
    // todo: handle all mesh types
}

Scene loadSceneFromFile(std::string fileName) {
    std::ifstream file(fileName);

    Scene scene;

    while (1) {
        Properties properties = loadProperties(file);

        if (file.fail()) break;

        addMesh(properties, scene);
    }

    file.close();

    return scene;
}

}  // namespace eng
