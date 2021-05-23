#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/Assets.h"
#include "Engine/Renderer.h"

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

    eng::MeshData skyboxMesh = eng::MeshData::generateCubeData(1.0f, true);

    float s = 2.6f;
    float h = -0.8;
    eng::MeshData planeMesh = eng::MeshData{{{-s, h, s}, {-s, h, -s}, {s, h, s}, {s, h, -s}},
                                            {{0.0f, 1.0f, 0.0f}},
                                            {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                            {{0, 2, 1, 0, 1, 3, 0, 0, 0}, {3, 1, 2, 2, 3, 1, 0, 0, 0}}};

    float sz = 0.6f;
    eng::MeshData cubeMesh = eng::MeshData::generateCubeData(sz);

    eng::Texture swordTexture = eng::Texture("data/sword.png");
    eng::Texture brickNormalMap = eng::Texture("data/brickwall_normal.jpg");

    sf::Font font;
    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
