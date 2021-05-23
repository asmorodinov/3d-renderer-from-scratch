#include "Assets.h"

namespace eng {

TextureRef Assets::getTexture() { return std::cref(Assets::get().texture); }

TextureRef Assets::getTexture(std::string filename) {
    auto& textures = Assets::get().textures;

    if (textures.find(filename) == textures.end()) {
        textures[filename] = Texture("data/textures/"s + filename);
    }

    return std::cref(textures[filename]);
}

CubemapTextureRef Assets::getCubemapTexture() { return std::cref(Assets::get().cubemapTexture); }

CubemapTextureRef Assets::getCubemapTexture(std::string filename, bool df, const std::string& f) {
    auto& cubemapTextures = Assets::get().cubemapTextures;

    std::string name = filename + std::to_string(df) + "_"s + f;

    if (cubemapTextures.find(name) == cubemapTextures.end()) {
        cubemapTextures[name] = CubemapTexture("data/cubemaps/"s + filename, df, f);
    }

    return std::cref(cubemapTextures[name]);
}

MeshDataRef Assets::getMeshData(std::string filename, float scale, bool in, bool ov) {
    auto& meshDatas = Assets::get().meshDatas;

    std::string name = filename + std::to_string(scale) + "_"s + std::to_string(in);

    if (meshDatas.find(name) == meshDatas.end()) {
        meshDatas[name] = eng::loadFromObj("data/models/"s + filename + ".obj"s, scale, in, ov);
    }

    return std::cref(meshDatas[name]);
}

}  // namespace eng
