#include "Texture.h"

namespace eng {

Texture::Texture() : textureWidth(0), textureHeight(0) {
}

Texture::Texture(const std::string& file) {
    fileName = file;

    int x = 0, y = 0, n = 0;
    unsigned char* data = stbi_load(("data/textures/" + file).c_str(), &x, &y, &n, 3);

    assert(x > 0 && y > 0 && n >= 3 && data);
    textureWidth = x;
    textureHeight = y;

    colorBuffer = ColorBuffer128(textureWidth, textureHeight, Color128(0.0f));

    for (size_t i = 0; i < textureWidth; ++i) {
        for (size_t j = 0; j < textureHeight; ++j) {
            unsigned char* pixel = data + (i + textureWidth * j) * 3;
            colorBuffer.set(i, j, Color128(pixel[0], pixel[1], pixel[2], 255.0f) / 255.0f);
        }
    }

    stbi_image_free(data);
}

glm::vec4 Texture::sample(glm::vec2 uv) const {
    Pixels xc = uv.s * textureWidth;
    Pixels yc = uv.t * textureHeight;
    xc = std::min(xc, textureWidth - 1);
    yc = std::min(yc, textureHeight - 1);

    return colorBuffer.get(xc, textureHeight - 1 - yc);
}

CubemapTexture::CubemapTexture() {
}

CubemapTexture::CubemapTexture(const std::string& folder, bool defaultFormat_, const std::string& f) {
    fileName = folder;
    auto filename2 = "../cubemaps/" + folder;

    defaultFormat = defaultFormat_;
    imageFormat = f;

    if (defaultFormat_) {
        textures[0] = Texture(filename2 + "/posx" + f);
        textures[1] = Texture(filename2 + "/negx" + f);
        textures[2] = Texture(filename2 + "/posy" + f);
        textures[3] = Texture(filename2 + "/negy" + f);
        textures[4] = Texture(filename2 + "/posz" + f);
        textures[5] = Texture(filename2 + "/negz" + f);
    } else {
        textures[0] = Texture(filename2 + "/right" + f);
        textures[1] = Texture(filename2 + "/left" + f);
        textures[2] = Texture(filename2 + "/top" + f);
        textures[3] = Texture(filename2 + "/bottom" + f);
        textures[4] = Texture(filename2 + "/front" + f);
        textures[5] = Texture(filename2 + "/back" + f);
    }
}

glm::vec4 CubemapTexture::sample(glm::vec3 direction) const {
    direction = glm::normalize(direction);
    if (textures[0].textureWidth == 0) {
        return glm::vec4(0.5f * (direction + 1.0f), 1.0f);
    } else {
        int index = 0;
        float u = 0.0f;
        float v = 0.0f;

        float x = direction.x;
        float y = direction.y;
        float z = direction.z;

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

        return textures[index].sample({u, v});
    }
}

}  // namespace eng
