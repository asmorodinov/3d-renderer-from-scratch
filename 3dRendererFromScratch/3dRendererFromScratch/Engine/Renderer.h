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

    Screen& getScreen();
    World& getWorld();

    void renderSceneToScreen();

    void renderScreenToFile(const std::string& file) const;

 private:
    Screen screen;
    World world;
};

}  // namespace eng
