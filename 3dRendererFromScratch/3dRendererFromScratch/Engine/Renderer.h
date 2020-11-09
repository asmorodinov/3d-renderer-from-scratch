#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <stb_image.h>
#include <stb_image_write.h>

#include "World.h"
#include "Screen.h"

namespace eng {

class Renderer {
 public:
    Renderer(size_t width, size_t height);

    Screen& getScreen() { return screen; }
    World& getWorld() { return world; }

    void renderSceneToScreen() {
        for (auto& object : world.getObjects()) object->draw(world.getCamera(), screen);
    }

    void renderScreenToFile(const std::string& file) const {
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

 private:
    Screen screen;
    World world;
};

}  // namespace eng
