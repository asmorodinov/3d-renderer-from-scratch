#pragma once

#include <cassert>
#include <string>
#include <array>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Screen.h"

namespace eng {

// If you want, you can find definition of the verb "sample" here: https://learnopengl.com/Getting-started/Textures
// (it's a commonly used 3d graphics term)

struct Texture {
    using Pixels = Screen::Pixels;
    using Color = Screen::Color;
    using ColorBuffer = Screen::ColorBuffer;

    Texture();
    Texture(const std::string& file);
    glm::vec4 sample(glm::vec2 uv) const;

    Pixels textureWidth, textureHeight;
    ColorBuffer colorBuffer;

    std::string fileName;
};

struct CubemapTexture {
    using FacesTextures = std::array<Texture, 6>;

    CubemapTexture();
    CubemapTexture(const std::string& folder, bool defaultFormat = true, const std::string& imageFormat = ".jpg");

    glm::vec4 sample(glm::vec3 direction) const;

    FacesTextures textures;

    std::string fileName;
    bool defaultFormat;
    std::string imageFormat;
};

}  // namespace eng
