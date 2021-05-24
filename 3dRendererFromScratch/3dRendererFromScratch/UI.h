#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/Assets.h"
#include "Engine/Renderer.h"
#include "Engine/SampleScenes.h"

class UI {
 public:
    UI(sf::RenderWindow& window_);

    void updateAndDraw(float dt, size_t trianglesCount = 0);

 private:
    std::reference_wrapper<sf::RenderWindow> window;

    int frames = 0;

    float time = 0.0f;
    float lastTime = 0.0f;
    float fps = 0.0f;

    sf::Font font;

    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
