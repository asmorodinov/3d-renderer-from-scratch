#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "World.h"
#include "Screen.h"

#include "../profile.h"

namespace eng {

class Renderer {
 public:
    Renderer(size_t width, size_t height);

    Screen& getScreen();
    World& getWorld();

    void clearScreen();

    void renderSceneToScreen();

    void renderScreenToFile(const std::string& file) const;
    void renderScreenToSFMLWindow(sf::RenderWindow& window);

 private:
    Screen screen;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;

    World world;
};

}  // namespace eng
