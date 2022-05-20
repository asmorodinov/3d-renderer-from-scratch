#pragma once

#include <cassert>
#include <string>
#include <array>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Vector2d.h"
#include "Types.h"

namespace eng {

// If you want, you can find definition of the verb "sample" here: https://learnopengl.com/Getting-started/Textures
// (it's a commonly used 3d graphics term)

template <typename T>
T sample2dBuffer(const Vector2d<T>& buf, glm::vec2 uv) {
    auto width = buf.getWidth();
    auto height = buf.getHeight();

    Pixels xc = uv.s * width;
    Pixels yc = uv.t * height;
    xc = std::min(xc, width - 1);
    yc = std::min(yc, height - 1);
    return buf.get(xc, height - 1 - yc);
}

struct Texture {
    Texture();
    Texture(const std::string& file);
    Color128 sample(glm::vec2 uv) const;

    Pixels textureWidth, textureHeight;
    ColorBuffer128 colorBuffer;

    std::string fileName;
};

struct CubemapTexture {
    using FacesTextures = std::array<Texture, 6>;

    CubemapTexture();
    CubemapTexture(const std::string& folder, bool defaultFormat = true, const std::string& imageFormat = ".jpg");

    Color128 sample(glm::vec3 direction) const;

    FacesTextures textures;

    std::string fileName;
    bool defaultFormat;
    std::string imageFormat;
};

}  // namespace eng
