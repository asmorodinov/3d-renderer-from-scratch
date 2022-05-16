#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/Renderer.h"
#include "Engine/Types.h"

class SFMLRenderer {
 public:
    using Pixels = eng::Pixels;

 public:
    SFMLRenderer(Pixels width, Pixels height, sf::RenderWindow& mainAppWindow);

    size_t render(eng::Scene& scene);

 private:
    Pixels windowWidth_;
    Pixels windowHeight_;

    std::reference_wrapper<sf::RenderWindow> mainAppWindow_;
    eng::Renderer renderer_;

    sf::Texture screenTexture_;
    sf::Sprite screenSprite_;
};
