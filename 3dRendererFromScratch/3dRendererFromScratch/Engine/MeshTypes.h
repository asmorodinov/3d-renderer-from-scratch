#pragma once

#include <map>
#include <variant>
#include <tuple>

#include "Shaders.h"
#include "Mesh.h"
#include "Camera.h"

namespace eng {

using FlatMesh = Mesh<FlatVertexShader, FlatShader>;
using TextureMesh = Mesh<TextureVertexShader, TextureShader>;
using CubemapMesh = Mesh<CubemapVertexShader, CubemapShader>;
using UVMesh = Mesh<UVVertexShader, UVShader>;
using NormalMesh = Mesh<NormalVertexShader, NormalShader>;
using PhongMesh = Mesh<PhongVertexShader, PhongShader>;
using NormalMapMesh = Mesh<NormalMapVertexShader, NormalMapShader>;

using MeshVariant = std::variant<FlatMesh, TextureMesh, CubemapMesh, UVMesh, NormalMesh, PhongMesh, NormalMapMesh>;

using ObjectsVec = std::map<std::string, MeshVariant>;

}  // namespace eng
