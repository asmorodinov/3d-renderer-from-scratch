#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "VertexShader.h"
#include "Mesh.h"
#include "Camera.h"

namespace eng {

using FlatMesh = Mesh<FlatVertexShader::Uniform, FlatShader::Uniform, FlatVertexShader::Output, FlatVertexShader, FlatShader>;
using TextureMesh =
    Mesh<TextureVertexShader::Uniform, TextureShader::Uniform, TextureVertexShader::Output, TextureVertexShader, TextureShader>;

using CubemapMesh =
    Mesh<CubemapVertexShader::Uniform, CubemapShader::Uniform, CubemapVertexShader::Output, CubemapVertexShader, CubemapShader>;

using UVMesh = Mesh<UVVertexShader::Uniform, UVShader::Uniform, UVVertexShader::Output, UVVertexShader, UVShader>;
using NormalMesh =
    Mesh<NormalVertexShader::Uniform, NormalShader::Uniform, NormalVertexShader::Output, NormalVertexShader, NormalShader>;

using PhongMesh =
    Mesh<PhongVertexShader::Uniform, PhongShader::Uniform, PhongVertexShader::Output, PhongVertexShader, PhongShader>;

using NormalMapMesh = Mesh<NormalMapVertexShader::Uniform, NormalMapShader::Uniform, NormalMapVertexShader::Output,
                           NormalMapVertexShader, NormalMapShader>;

struct ObjectsVec {
    std::vector<FlatMesh> flatMeshes;
    std::vector<TextureMesh> textureMeshes;
    std::vector<CubemapMesh> cubemapMeshes;
    std::vector<UVMesh> uvMeshes;
    std::vector<NormalMesh> normalMeshes;
    std::vector<PhongMesh> phongMeshes;
    std::vector<NormalMapMesh> normalMapMeshes;
};

class Scene {
 public:
    Scene(){};

    const Camera& getCamera() const;
    Camera& getCamera();

    const ObjectsVec& getObjects() const;
    ObjectsVec& getObjects();

    const LightsVec& getPointLights() const;
    LightsVec& getPointLights();

 private:
    Camera camera;
    ObjectsVec objects;
    LightsVec pointLights;
};

}  // namespace eng
