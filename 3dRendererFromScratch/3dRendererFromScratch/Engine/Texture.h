#pragma once

#include <cassert>
#include <string>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Screen.h"

namespace eng {

struct Texture {
    Texture();
    Texture(const std::string& file);
    glm::vec4 sample(float x, float y) const;

    size_t w, h;
    ColorBuffer buffer;
};

struct CubemapTexture {
    CubemapTexture();
    CubemapTexture(const std::string& folder);

    Texture textures[6];

    glm::vec4 sample(glm::vec3 v);
};

}  // namespace eng
