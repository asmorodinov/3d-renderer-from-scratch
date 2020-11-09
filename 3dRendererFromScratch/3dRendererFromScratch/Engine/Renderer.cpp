#include "Renderer.h"

namespace eng {

Renderer::Renderer(size_t width, size_t height) : screen(width, height, {0.3f, 0.85f, 0.45f}) {}

Screen& Renderer::getScreen() { return screen; }
World& Renderer::getWorld() { return world; }

void Renderer::renderSceneToScreen() {
    for (auto& object : world.getObjects()) object->draw(world.getCamera(), screen);
}

void Renderer::renderScreenToFile(const std::string& file) const {
    size_t width = screen.getWidth();
    size_t height = screen.getHeight();

    std::vector<uint8_t> pixels(width * height * 3, 0);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            ColorType color = screen.getPixelColor(x, y);

            pixels[3 * (y * width + x) + 0] = uint8_t(255.99f * color.r);
            pixels[3 * (y * width + x) + 1] = uint8_t(255.99f * color.g);
            pixels[3 * (y * width + x) + 2] = uint8_t(255.99f * color.b);
        }
    }

    stbi_write_png(file.c_str(), width, height, 3, &pixels[0], width * 3);
}

}  // namespace eng
