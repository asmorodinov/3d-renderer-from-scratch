#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "World.h"
#include "Screen.h"
#include "PlayerCamera.h"

#include "../profile.h"

namespace eng {

class Renderer {
 public:
    Renderer(size_t width, size_t height);

    Screen& getScreen();
    World& getWorld();

    void clearScreen();

    void mouseMove(float x, float y);
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode);

    void update(float dt);
    size_t renderSceneToScreen();

    void renderScreenToFile(const std::string& file) const;
    void renderScreenToSFMLWindow(sf::RenderWindow& window);

    void setRenderMode(RenderMode r);
    RenderMode getRenderMode() const;

 private:
    Screen screen;
    sf::Texture screenTexture;
    sf::Sprite screenSprite;
    World world;
    PlayerCamera camera;

    bool playerControl = true;
    float mx = -1.0f, my = -1.0f;

    RenderMode rm;
};

}  // namespace eng
