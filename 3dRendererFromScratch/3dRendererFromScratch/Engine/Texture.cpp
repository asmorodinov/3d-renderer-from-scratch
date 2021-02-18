#include "Texture.h"

namespace eng {

Texture::Texture() : w(0), h(0) {}

Texture::Texture(const std::string& file) {
    int x = 0, y = 0, n = 0;
    unsigned char* data = stbi_load(file.c_str(), &x, &y, &n, 3);

    assert(x > 0 && y > 0 && n >= 3 && data);
    w = x;
    h = y;

    buffer = ColorBuffer(w, h, ColorType(0.0f));

    for (size_t i = 0; i < w; ++i) {
        for (size_t j = 0; j < h; ++j) {
            unsigned char* pixel = data + (i + w * j) * 3;
            buffer.set(i, j, ColorType(pixel[0], pixel[1], pixel[2]) / float(255));
        }
    }

    stbi_image_free(data);
}

glm::vec4 Texture::sample(float x, float y) const {
    x = std::max(0.0f, std::min(x, 1.0f));
    y = std::max(0.0f, std::min(y, 1.0f));

    size_t xc = size_t(x * w);
    size_t yc = size_t(y * h);
    xc = std::min(xc, w - 1);
    yc = std::min(yc, h - 1);

    return glm::vec4(buffer.get(xc, h - 1 - yc), 1.0f);
}

CubemapTexture::CubemapTexture() {}

CubemapTexture::CubemapTexture(const std::string& folder, bool defaultFormat, const std::string& f) {
    if (defaultFormat) {
        textures[0] = Texture(folder + "/posx" + f);
        textures[1] = Texture(folder + "/negx" + f);
        textures[2] = Texture(folder + "/posy" + f);
        textures[3] = Texture(folder + "/negy" + f);
        textures[4] = Texture(folder + "/posz" + f);
        textures[5] = Texture(folder + "/negz" + f);
    } else {
        textures[0] = Texture(folder + "/right" + f);
        textures[1] = Texture(folder + "/left" + f);
        textures[2] = Texture(folder + "/top" + f);
        textures[3] = Texture(folder + "/bottom" + f);
        textures[4] = Texture(folder + "/front" + f);
        textures[5] = Texture(folder + "/back" + f);
    }
}

glm::vec4 CubemapTexture::sample(glm::vec3 vec) {
    vec = glm::normalize(vec);
    if (textures[0].w == 0) {
        return glm::vec4(0.5f * (vec + 1.0f), 1.0f);
    } else {
        int index = 0;
        float u = 0.0f;
        float v = 0.0f;

        float x = vec.x;
        float y = vec.y;
        float z = vec.z;

        float absX = std::abs(x);
        float absY = std::abs(y);
        float absZ = std::abs(z);

        int isXPositive = x > 0 ? 1 : 0;
        int isYPositive = y > 0 ? 1 : 0;
        int isZPositive = z > 0 ? 1 : 0;

        float maxAxis = 1.0f, uc = 0.0f, vc = 0.0f;

        // POSITIVE X
        if (isXPositive && absX >= absY && absX >= absZ) {
            // u (0 to 1) goes from +z to -z
            // v (0 to 1) goes from -y to +y
            maxAxis = absX;
            uc = -z;
            vc = y;
            index = 0;
        }
        // NEGATIVE X
        if (!isXPositive && absX >= absY && absX >= absZ) {
            // u (0 to 1) goes from -z to +z
            // v (0 to 1) goes from -y to +y
            maxAxis = absX;
            uc = z;
            vc = y;
            index = 1;
        }
        // POSITIVE Y
        if (isYPositive && absY >= absX && absY >= absZ) {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from +z to -z
            maxAxis = absY;
            uc = x;
            vc = -z;
            index = 2;
        }
        // NEGATIVE Y
        if (!isYPositive && absY >= absX && absY >= absZ) {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -z to +z
            maxAxis = absY;
            uc = x;
            vc = z;
            index = 3;
        }
        // POSITIVE Z
        if (isZPositive && absZ >= absX && absZ >= absY) {
            // u (0 to 1) goes from -x to +x
            // v (0 to 1) goes from -y to +y
            maxAxis = absZ;
            uc = x;
            vc = y;
            index = 4;
        }
        // NEGATIVE Z
        if (!isZPositive && absZ >= absX && absZ >= absY) {
            // u (0 to 1) goes from +x to -x
            // v (0 to 1) goes from -y to +y
            maxAxis = absZ;
            uc = -x;
            vc = y;
            index = 5;
        }

        // Convert range from -1 to 1 to 0 to 1
        u = 0.5f * (uc / maxAxis + 1.0f);
        v = 0.5f * (vc / maxAxis + 1.0f);

        return textures[index].sample(u, v);
    }
}

}  // namespace eng
