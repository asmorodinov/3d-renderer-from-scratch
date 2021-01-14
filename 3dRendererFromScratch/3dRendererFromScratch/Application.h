#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "Engine/Renderer.h"

class Application {
 public:
    Application();
    void initInterface();
    void initShaders();
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

    eng::Shader flatShader;
    eng::Shader textureShader;
    eng::Shader uvShader;
    eng::Shader normalShader;
    eng::Shader phongShader;
    eng::Shader skyboxShader;

    eng::Texture texture = eng::Texture("data/texture.png");
    eng::Texture texture2 = eng::Texture("data/texture2.png");
    eng::Texture texture3 = eng::Texture("data/texture3.png");
    eng::Texture swordTexture = eng::Texture("data/sword.png");

    eng::CubemapTexture skybox = eng::CubemapTexture();

    sf::Font font;
    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
