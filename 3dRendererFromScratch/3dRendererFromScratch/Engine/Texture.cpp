#include "Texture.h"

namespace eng {

Texture::Texture() {}

Texture::Texture(const std::string& file) {
    int x = 0, y = 0, n = 0;
    unsigned char* data = stbi_load(file.c_str(), &x, &y, &n, 3);

    assert(x > 0 && y > 0 && n >= 3 && data);
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

glm::vec4 Texture::sample(float x, float y, int n, int m) const {
    // temporary hack, this will be removed when I add triangle clipping with frustrum planes
    // (e.g. to not render objects behind camera)
    static constexpr float eps = 0.01f;
    if (x < -eps || y < -eps || x > 1 + eps || y > 1 + eps) return glm::vec4();

    x = (x * n) - int(x * n);
    y = (y * m) - int(y * m);

    size_t xc = size_t(x * (w - 1));
    size_t yc = size_t(y * (h - 1));
    xc = std::max(std::min(xc, w - 1), size_t(0));
    yc = std::max(std::min(yc, h - 1), size_t(0));

    return glm::vec4(buffer[xc][h - 1 - yc], 1.0f);
}

}  // namespace eng
