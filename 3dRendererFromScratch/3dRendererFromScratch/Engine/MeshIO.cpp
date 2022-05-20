#include "MeshIO.h"

namespace eng {

Properties loadProperties(std::ifstream& file) {
    Properties res;

    bool start = false;

    while (1) {
        std::string name;
        file >> name;

        if (file.fail()) break;

        if (name == "{") {
            start = true;
            continue;
        }

        if (!start) continue;

        if (name == "}") break;

        bool correctProperty = false;

        READ_MEMBER(name, std::string)
        READ_MEMBER(typeName, std::string)

        READ_MEMBER(diffuseTextureName, std::string)
        READ_MEMBER(normalTextureName, std::string)
        READ_MEMBER(displacementTextureName, std::string)

        READ_MEMBER(cubemapTextureName, std::string)
        READ_MEMBER(meshFileName, std::string)
        READ_MEMBER(cubemapImageFormat, std::string)
        READ_MEMBER(meshScale, float)
        READ_MEMBER(meshInvertNormals, bool)
        READ_MEMBER(meshOnlyVertices, bool)
        READ_MEMBER(wireframeMode, bool)
        READ_MEMBER(writeToDepthBuffer, bool)
        READ_MEMBER(drawingEnabled, bool)
        READ_MEMBER(isTransparent, bool)
        READ_MEMBER(drawBackface, bool)
        READ_MEMBER(cubemapDefaultFormat, bool)
        READ_VEC3_MEMBER(transformPosition)
        READ_VEC3_MEMBER(transformScale)
        READ_VEC3_MEMBER(wireframeColor)
        READ_VEC3_MEMBER(flatColor)

        assert(correctProperty);
    }

    return res;
}

PointLight loadPointLightFromFile(std::ifstream& file) {
    PointLight res;

    bool start = false;

    while (1) {
        std::string name;
        file >> name;

        if (file.fail()) break;

        if (name == "{") {
            start = true;
            continue;
        }

        if (!start) continue;

        if (name == "}") break;

        bool correctProperty = false;

        READ_VEC3_MEMBER(position)
        READ_VEC3_MEMBER(color)
        READ_MEMBER(intensity, float)
        READ_MEMBER(specularCoefficient, float)
        READ_MEMBER(diffuseCoefficient, float)
        READ_MEMBER(linearAttenuationCoefficient, float)
        READ_MEMBER(quadraticAttenuationCoefficient, float)
        READ_MEMBER(cubicAttenuationCoefficient, float)

        assert(correctProperty);
    }

    return res;
}

CubemapTextureRef getCubemapTextureFromProperties(const Properties& pr) {
    return Assets::getCubemapTexture(pr.cubemapTextureName, pr.cubemapDefaultFormat, pr.cubemapImageFormat);
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
    ADD_MESH(ParallaxMapMesh);

    assert(correctMeshType);
}

Scene loadSceneFromFile(std::string fileName) {
    std::ifstream file(std::string("data/scenes/") + fileName + ".scn");

    if (file.fail()) {
        return Scene("empty");
    }

    Scene scene(fileName);

    while (1) {
        std::string objectType;
        file >> objectType;

        if (file.fail()) break;

        if (objectType == "Mesh") {
            Properties properties = loadProperties(file);

            if (file.fail()) break;

            addMesh(properties, scene);
        } else if (objectType == "PointLight") {
            PointLight light = loadPointLightFromFile(file);

            if (file.fail()) break;

            scene.getPointLights().push_back(light);
        }
    }

    file.close();

    return scene;
}

bool saveSceneToFile(const Scene& scene, std::string fileName) {
    std::ofstream file(std::string("data/scenes/") + fileName + ".scn");

    if (!file) {
        return false;
    }

    for (const auto& [name, meshVariant] : scene.getAllObjects()) {
        Properties properties = getMeshProperties(meshVariant, name);
        Properties defaultProperties = Properties();

        file << "Mesh\n";
        file << "{\n";

        WRITE_MEMBER(name)
        WRITE_MEMBER(typeName)

        WRITE_MEMBER(diffuseTextureName)
        WRITE_MEMBER(normalTextureName)
        WRITE_MEMBER(displacementTextureName)

        WRITE_MEMBER(cubemapTextureName)
        WRITE_MEMBER(meshFileName)
        WRITE_MEMBER(cubemapImageFormat)
        WRITE_MEMBER(meshScale)
        WRITE_MEMBER(meshInvertNormals)
        WRITE_MEMBER(meshOnlyVertices)
        WRITE_MEMBER(wireframeMode)
        WRITE_MEMBER(writeToDepthBuffer)
        WRITE_MEMBER(drawingEnabled)
        WRITE_MEMBER(isTransparent)
        WRITE_MEMBER(drawBackface)
        WRITE_MEMBER(cubemapDefaultFormat)
        WRITE_VEC3_MEMBER(transformPosition)
        WRITE_VEC3_MEMBER(transformScale)
        WRITE_VEC3_MEMBER(wireframeColor)
        WRITE_VEC3_MEMBER(flatColor)

        file << "}\n";
    }

    for (const auto& light : scene.getPointLights()) {
        PointLight properties = light;
        PointLight defaultProperties = PointLight();

        file << "PointLight\n";
        file << "{\n";

        WRITE_VEC3_MEMBER(position)
        WRITE_VEC3_MEMBER(color)
        WRITE_MEMBER(intensity)
        WRITE_MEMBER(specularCoefficient)
        WRITE_MEMBER(diffuseCoefficient)
        WRITE_MEMBER(linearAttenuationCoefficient)
        WRITE_MEMBER(quadraticAttenuationCoefficient)
        WRITE_MEMBER(cubicAttenuationCoefficient)

        file << "}\n";
    }

    file.close();

    return true;
}

Properties getMeshProperties(const MeshVariant& mesh, std::string name) {
    return std::visit([&](auto&& arg) -> Properties { return getProperties(arg, name); }, mesh);
}

}  // namespace eng
