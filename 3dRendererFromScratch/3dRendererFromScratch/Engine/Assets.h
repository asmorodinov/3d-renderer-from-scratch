#pragma once

#include <array>
#include <iostream>
#include <string>
#include <map>
#include <memory_resource>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Common.h"

namespace eng {

using namespace std::string_literals;

using TextureRef = std::reference_wrapper<const Texture>;
using CubemapTextureRef = std::reference_wrapper<const CubemapTexture>;
using MeshDataRef = std::reference_wrapper<const MeshData>;

class Assets {
 public:
    static Assets& get() {
        static Assets me;
        return me;
    }

    Assets(Assets const&) = delete;
    void operator=(Assets const&) = delete;

    static TextureRef getTexture();
    static TextureRef getTexture(std::string filename);

    static CubemapTextureRef getCubemapTexture();
    static CubemapTextureRef getCubemapTexture(std::string filename, bool defaultFormat = true,
                                               const std::string& imageFormat = ".jpg"s);

    static MeshDataRef getMeshData(std::string filename, float scale = 1.0f, bool in = false, bool ov = false);

    Texture texture = Texture();
    CubemapTexture cubemapTexture = CubemapTexture();

 private:
    std::array<char, 300 * 1024 * 1024> buffer = {};
    std::pmr::monotonic_buffer_resource pool{std::data(buffer), std::size(buffer)};

    std::pmr::map<std::string, Texture> textures{&pool};
    std::pmr::map<std::string, CubemapTexture> cubemapTextures{&pool};
    std::pmr::map<std::string, MeshData> meshDatas{&pool};
    Assets() = default;
};

}  // namespace eng
