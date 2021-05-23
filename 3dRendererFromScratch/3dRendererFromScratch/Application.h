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

class Application {
 private:
    using Pixels = std::size_t;
    static constexpr const Pixels default_width = 1200;
    static constexpr const Pixels default_height = 800;
    static constexpr const char* mainWindowMsg_ = "SFML window [press SPACE to toggle rendering modes]";

 public:
    Application();
    void initInterface();
    void addObjects();

    void processEvents();
    void run();

 private:
    size_t width;
    size_t height;
    sf::RenderWindow window;
    eng::Renderer renderer;

    int frames = 0;
    float lastTime = 0.0f;
    float fps = 0.0;

    sf::Font font;
    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
