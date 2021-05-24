#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/Renderer.h"

class SFMLRenderer {
 public:
    SFMLRenderer(size_t width, size_t height, sf::RenderWindow& window);

    size_t render(eng::Scene& scene);

 private:
    size_t width;
    size_t height;

    std::reference_wrapper<sf::RenderWindow> window;
    eng::Renderer renderer;

    sf::Texture screenTexture;
    sf::Sprite screenSprite;
};
