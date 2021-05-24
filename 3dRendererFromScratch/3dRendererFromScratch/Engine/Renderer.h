#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Scene.h"
#include "Screen.h"

namespace eng {

class Renderer {
 public:
    Renderer(size_t width, size_t height);

    Screen& getScreen();

    void clearScreen();

    size_t renderSceneToScreen(Scene& scene);

    void renderScreenToFile(const std::string& file) const;

 private:
    Screen screen;
};

}  // namespace eng
