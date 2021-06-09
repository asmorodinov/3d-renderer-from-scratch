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

        READ_MEMBER(name, std::string)
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
        READ_MEMBER(drawingEnabled, bool)
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
        scene.addObject(pr.name, createFlatMesh(pr));
    } else if (typeName == "TextureMesh") {
        scene.addObject(pr.name, createTextureMesh(pr));
    } else if (typeName == "CubemapMesh") {
        scene.addObject(pr.name, createCubemapMesh(pr));
    }
    // todo: handle all mesh types
}

Scene loadSceneFromFile(std::string fileName) {
    std::ifstream file(std::string("data/scenes/") + fileName);

    Scene scene;

    while (1) {
        Properties properties = loadProperties(file);

        if (file.fail()) break;

        addMesh(properties, scene);
    }

    file.close();

    return scene;
}

// get properties
Properties getMeshProperties(const FlatMesh& mesh, std::string name) {
    return getProperties<FlatMesh>(
        mesh, [](const FlatMesh::VertexShaderUniform& uniform, Properties& pr) {},
        [](const FlatMesh::FragmentShaderUniform& uniform, Properties& pr) { pr.flatColor = uniform; }, "FlatMesh", name);
}

Properties getMeshProperties(const TextureMesh& mesh, std::string name) {
    return getProperties<TextureMesh>(
        mesh, [](const TextureMesh::VertexShaderUniform& uniform, Properties& pr) {},
        [](const TextureMesh::FragmentShaderUniform& uniform, Properties& pr) { pr.diffuseTextureName = uniform.get().fileName; }, "TextureMesh", name);
}

Properties getMeshProperties(const CubemapMesh& mesh, std::string name) {
    return getProperties<CubemapMesh>(
        mesh, [](const CubemapMesh::VertexShaderUniform& uniform, Properties& pr) {},
        [](const CubemapMesh::FragmentShaderUniform& uniform, Properties& pr) {
            pr.cubemapTextureName = uniform.get().fileName;
            pr.cubemapDefaultFormat = uniform.get().defaultFormat;
            pr.cubemapImageFormat = uniform.get().imageFormat;
        },
        "CubemapMesh", name);
}

Properties getMeshProperties(const MeshVariant& mesh, std::string name) {
    return std::visit([&](auto&& arg) -> Properties { return getMeshProperties(arg, name); }, mesh);
}

}  // namespace eng
