#pragma once

#include <assert.h>
#include <string>
#include <vector>

#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "Screen.h"

namespace eng {

struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat orientation = glm::quat();
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 getModel() const;
};

struct Texture {
    Texture(const std::string& file) {
        int x = 0, y = 0, n = 0;
        unsigned char* data = stbi_load(file.c_str(), &x, &y, &n, 3);

        assert(x > 0 && y > 0 && n == 3 && data);
        w = x;
        h = y;

        buffer = ColorBuffer(w, std::vector<ColorType>(h, ColorType(0.0f)));

        for (size_t i = 0; i < w; ++i) {
            for (size_t j = 0; j < h; ++j) {
                unsigned char* pixel = data + (i + w * j) * 3;
                buffer[i][j] = ColorType(pixel[0], pixel[1], pixel[2]) / float(255);
            }
        }

        stbi_image_free(data);
    }
    ColorType sample(float x, float y) {
        assert(x >= 0 && x <= 1 && y >= 0 && y <= 1);
        size_t xc = size_t(x * (w - 1));
        size_t yc = size_t(y * (h - 1));

        return buffer[xc][h - 1 - yc];
    }

    size_t w, h;
    ColorBuffer buffer;
};

}  // namespace eng
