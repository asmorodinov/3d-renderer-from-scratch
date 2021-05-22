#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Mesh.h"

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
    std::unordered_map<std::string, Texture> textures;
    std::unordered_map<std::string, CubemapTexture> cubemapTextures;
    std::unordered_map<std::string, MeshData> meshDatas;
    Assets() = default;
};

}  // namespace eng
