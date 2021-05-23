#pragma once

#include <tuple>

#include "VertexShader.h"
#include "Mesh.h"
#include "Camera.h"

namespace eng {

using FlatMesh = Mesh<FlatVertexShader::Uniform, FlatShader::Uniform, FlatVertexShader::Output, FlatVertexShader, FlatShader>;
using TextureMesh = Mesh<TextureVertexShader::Uniform, TextureShader::Uniform, TextureVertexShader::Output, TextureVertexShader, TextureShader>;
using CubemapMesh = Mesh<CubemapVertexShader::Uniform, CubemapShader::Uniform, CubemapVertexShader::Output, CubemapVertexShader, CubemapShader>;
using UVMesh = Mesh<UVVertexShader::Uniform, UVShader::Uniform, UVVertexShader::Output, UVVertexShader, UVShader>;
using NormalMesh = Mesh<NormalVertexShader::Uniform, NormalShader::Uniform, NormalVertexShader::Output, NormalVertexShader, NormalShader>;
using PhongMesh = Mesh<PhongVertexShader::Uniform, PhongShader::Uniform, PhongVertexShader::Output, PhongVertexShader, PhongShader>;
using NormalMapMesh = Mesh<NormalMapVertexShader::Uniform, NormalMapShader::Uniform, NormalMapVertexShader::Output, NormalMapVertexShader, NormalMapShader>;

using ObjectsVec = std::tuple<std::pmr::vector<FlatMesh>, std::pmr::vector<TextureMesh>, std::pmr::vector<CubemapMesh>, std::pmr::vector<UVMesh>,
                              std::pmr::vector<NormalMesh>, std::pmr::vector<PhongMesh>, std::pmr::vector<NormalMapMesh>>;

}  // namespace eng
