#pragma once

#include <string>
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
using ParallaxMapMesh = Mesh<ParallaxMapVertexShader, ParallaxMapShader>;

using MeshVariant = std::variant<FlatMesh, TextureMesh, CubemapMesh, UVMesh, NormalMesh, PhongMesh, NormalMapMesh, ParallaxMapMesh>;

using ObjectsVec = std::map<std::string, MeshVariant>;

// define type names with macros and some template magic
template <typename T>
struct TypeNameMap {
    static const std::string name;
};

template <typename T>
const std::string TypeNameMap<T>::name = "default name";

#define SPECIFY_TYPE_NAME_FOR_SOME_TYPE(type) \
    template <>                               \
    const std::string TypeNameMap<##type##>::name = #type

SPECIFY_TYPE_NAME_FOR_SOME_TYPE(FlatMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(TextureMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(CubemapMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(UVMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(NormalMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(PhongMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(NormalMapMesh);
SPECIFY_TYPE_NAME_FOR_SOME_TYPE(ParallaxMapMesh);

}  // namespace eng
