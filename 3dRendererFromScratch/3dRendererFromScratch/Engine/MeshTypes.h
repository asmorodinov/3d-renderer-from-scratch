#pragma once

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

using ObjectsVec = std::tuple<std::pmr::vector<CubemapMesh>, std::pmr::vector<TextureMesh>, std::pmr::vector<FlatMesh>, std::pmr::vector<UVMesh>,
                              std::pmr::vector<NormalMesh>, std::pmr::vector<PhongMesh>, std::pmr::vector<NormalMapMesh>>;

}  // namespace eng
