#pragma once

#include <array>
#include <iostream>
#include <string>
#include <map>
#include <memory_resource>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "MeshData.h"

#include "Pool.h"

namespace eng {

using namespace std::string_literals;

using TextureRef = std::reference_wrapper<const Texture>;
using CubemapTextureRef = std::reference_wrapper<const CubemapTexture>;
using MeshDataRef = std::reference_wrapper<const MeshData>;

class Assets {
 public:
    static Assets& get();

    Assets(Assets const&) = delete;
    void operator=(Assets const&) = delete;

    static TextureRef getTexture();
    static TextureRef getTexture(std::string filename);

    static CubemapTextureRef getCubemapTexture();
    static CubemapTextureRef getCubemapTexture(std::string filename, bool defaultFormat = true, const std::string& imageFormat = ".jpg"s);

    static MeshDataRef getMeshData(std::string filename, float scale = 1.0f, bool in = false, bool ov = false);

 private:
    Texture texture = Texture();
    CubemapTexture cubemapTexture = CubemapTexture();

    std::pmr::map<std::string, Texture> textures_{Pool::getPool()};
    std::pmr::map<std::string, CubemapTexture> cubemapTextures_{Pool::getPool()};
    std::pmr::map<std::string, MeshData> meshDatas_{Pool::getPool()};

    Assets() = default;
};

}  // namespace eng
