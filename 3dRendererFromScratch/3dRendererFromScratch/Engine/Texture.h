#pragma once

#include <cassert>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

#include "Screen.h"

namespace eng {

struct Texture {
    Texture();
    Texture(const std::string& file);
    ColorType sample(float x, float y) const;

    size_t w, h;
    ColorBuffer buffer;
};

}  // namespace eng
