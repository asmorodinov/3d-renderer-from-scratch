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
    using Color = eng::Renderer::Color;

    SFMLRenderer(size_t width, size_t height, sf::RenderWindow& mainAppWindow);

    size_t render(eng::Scene& scene);

 private:
    size_t windowWidth_;
    size_t windowHeight_;

    std::reference_wrapper<sf::RenderWindow> mainAppWindow_;
    eng::Renderer renderer_;

    sf::Texture screenTexture_;
    sf::Sprite screenSprite_;
};
