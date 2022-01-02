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

UVMesh createUVMesh(const Properties& pr) {
    return createMesh<UVMesh>(
        pr, [](const Properties& pr) -> UVMesh::VertexShaderUniform { return {}; }, [](const Properties& pr) -> UVMesh::FragmentShaderUniform { return {}; });
}

NormalMesh createNormalMesh(const Properties& pr) {
    return createMesh<NormalMesh>(
        pr, [](const Properties& pr) -> NormalMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> NormalMesh::FragmentShaderUniform { return {}; });
}

PhongMesh createPhongMesh(const Properties& pr) {
    return createMesh<PhongMesh>(
        pr, [](const Properties& pr) -> PhongMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> PhongMesh::FragmentShaderUniform { return Assets::getTexture(pr.diffuseTextureName); });
}

NormalMapMesh createNormalMapMesh(const Properties& pr) {
    return createMesh<NormalMapMesh>(
        pr, [](const Properties& pr) -> NormalMapMesh::VertexShaderUniform { return {}; },
        [](const Properties& pr) -> NormalMapMesh::FragmentShaderUniform {
            return {Assets::getTexture(pr.diffuseTextureName), Assets::getTexture(pr.normalTextureName)};
        });
}

void addMesh(const Properties& pr, Scene& scene) {
    std::string typeName = pr.typeName;

    bool correctMeshType = false;

    ADD_MESH(FlatMesh);
    ADD_MESH(TextureMesh);
    ADD_MESH(CubemapMesh);
    ADD_MESH(UVMesh);
    ADD_MESH(NormalMesh);
    ADD_MESH(PhongMesh);
    ADD_MESH(NormalMapMesh);

    assert(correctMeshType);
}

static bool endsWith(std::string_view str, std::string_view suffix) {
    return str.size() >= suffix.size() && (0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix));
}

Scene loadSceneFromFile(std::string fileName) {
    const auto suf = std::string(".scn");
    assert(endsWith(fileName, suf));

    std::ifstream file(std::string("data/scenes/") + fileName);

    if (!file) {
        return Scene("empty");
    }

    std::string sceneName = fileName.substr(0, fileName.size() - suf.size());

    Scene scene(sceneName);

    while (1) {
        Properties properties = loadProperties(file);

        if (file.fail()) break;

        addMesh(properties, scene);
    }

    file.close();

    return scene;
}

bool saveSceneToFile(const Scene& scene, std::string fileName) {
    const auto suf = std::string(".scn");
    assert(endsWith(fileName, suf));

    std::ofstream file(std::string("data/scenes/") + fileName);

    if (!file) {
        return false;
    }

    for (const auto& [name, meshVariant] : scene.getAllObjects()) {
        Properties properties = getMeshProperties(meshVariant, name);
        Properties defaultProperties = Properties();

        file << "{\n";

        WRITE_MEMBER(name)
        WRITE_MEMBER(typeName)
        WRITE_MEMBER(diffuseTextureName)
        WRITE_MEMBER(normalTextureName)
        WRITE_MEMBER(cubemapTextureName)
        WRITE_MEMBER(meshFileName)
        WRITE_MEMBER(cubemapImageFormat)
        WRITE_MEMBER(meshScale)
        WRITE_MEMBER(meshInvertNormals)
        WRITE_MEMBER(meshOnlyVertices)
        WRITE_MEMBER(wireframeMode)
        WRITE_MEMBER(writeToDepthBuffer)
        WRITE_MEMBER(drawingEnabled)
        WRITE_MEMBER(cubemapDefaultFormat)
        WRITE_VEC3_MEMBER(transformPosition)
        WRITE_VEC3_MEMBER(transformScale)
        WRITE_VEC3_MEMBER(wireframeColor)
        WRITE_VEC3_MEMBER(flatColor)

        file << "}\n";
    }

    file.close();

    return true;
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

Properties getMeshProperties(const UVMesh& mesh, std::string name) {
    return getProperties<UVMesh>(
        mesh, [](const UVMesh::VertexShaderUniform& uniform, Properties& pr) {}, [](const UVMesh::FragmentShaderUniform& uniform, Properties& pr) {}, "UVMesh",
        name);
}

Properties getMeshProperties(const NormalMesh& mesh, std::string name) {
    return getProperties<NormalMesh>(
        mesh, [](const NormalMesh::VertexShaderUniform& uniform, Properties& pr) {}, [](const NormalMesh::FragmentShaderUniform& uniform, Properties& pr) {},
        "NormalMesh", name);
}

Properties getMeshProperties(const PhongMesh& mesh, std::string name) {
    return getProperties<PhongMesh>(
        mesh, [](const PhongMesh::VertexShaderUniform& uniform, Properties& pr) {},
        [](const PhongMesh::FragmentShaderUniform& uniform, Properties& pr) { pr.diffuseTextureName = uniform.get().fileName; }, "PhongMesh", name);
}

Properties getMeshProperties(const NormalMapMesh& mesh, std::string name) {
    return getProperties<NormalMapMesh>(
        mesh, [](const NormalMapMesh::VertexShaderUniform& uniform, Properties& pr) {},
        [](const NormalMapMesh::FragmentShaderUniform& uniform, Properties& pr) {
            pr.diffuseTextureName = uniform.diffuseMap.get().fileName;
            pr.normalTextureName = uniform.normalMap.get().fileName;
        },
        "NormalMapMesh", name);
}

Properties getMeshProperties(const MeshVariant& mesh, std::string name) {
    return std::visit([&](auto&& arg) -> Properties { return getMeshProperties(arg, name); }, mesh);
}

}  // namespace eng
