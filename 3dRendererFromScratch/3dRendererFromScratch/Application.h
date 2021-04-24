#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/Renderer.h"

class Application {
 private:
    using Pixels = std::size_t;
    static constexpr const Pixels default_width = 800;
    static constexpr const Pixels default_height = 600;
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

    eng::RenderMode rm;

    int frames = 0;
    float lastTime = 0.0f;
    float fps = 0.0;

    eng::FlatShader flatShader;
    eng::TextureShader textureShader;
    eng::UVShader uvShader;
    eng::NormalShader normalShader;
    eng::PhongShader phongShader;
    eng::CubemapShader skyboxShader;

    std::shared_ptr<eng::Texture> texture = std::make_shared<eng::Texture>("data/texture.png");
    std::shared_ptr<eng::Texture> texture2 = std::make_shared<eng::Texture>("data/texture2.png");
    std::shared_ptr<eng::Texture> texture3 = std::make_shared<eng::Texture>("data/texture3.png");
    std::shared_ptr<eng::Texture> swordTexture = std::make_shared<eng::Texture>("data/sword.png");

    std::shared_ptr<eng::CubemapTexture> skybox = std::make_shared<eng::CubemapTexture>("data/space", false, ".png");

    sf::Font font;
    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
