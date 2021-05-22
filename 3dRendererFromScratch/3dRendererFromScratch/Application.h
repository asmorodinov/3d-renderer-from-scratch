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

    eng::FlatShader flatShader;
    eng::TextureShader textureShader;
    eng::UVShader uvShader;
    eng::NormalShader normalShader;
    eng::PhongShader phongShader;
    eng::CubemapShader skyboxShader;
    eng::NormalMapShader normalMapShader;

    eng::FlatVertexShader flatVertexShader;
    eng::TextureVertexShader textureVertexShader;
    eng::UVVertexShader uvVertexShader;
    eng::NormalVertexShader normalVertexShader;
    eng::PhongVertexShader phongVertexShader;
    eng::CubemapVertexShader skyboxVertexShader;
    eng::NormalMapVertexShader normalMapVertexShader;

    eng::MeshData skyboxMesh = eng::MeshData::generateCubeData(1.0f, false);

    float s = 2.6f;
    float h = -0.8;
    eng::MeshData planeMesh = eng::MeshData{{{-s, h, s}, {-s, h, -s}, {s, h, s}, {s, h, -s}},
                                            {{0.0f, 1.0f, 0.0f}},
                                            {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                            {{0, 1, 2, 0, 3, 1, 0, 0, 0}, {3, 2, 1, 2, 1, 3, 0, 0, 0}}};

    float sz = 0.6f;
    eng::MeshData cubeMesh = eng::MeshData::generateCubeData(sz, true);

    eng::MeshData teapotMesh = eng::loadFromObj("data/teapot.obj", 0.4f, true, true);
    eng::MeshData sphereMesh = eng::loadFromObj("data/lowPolySphere.obj", 0.4f, true);
    eng::MeshData swordMesh = eng::loadFromObj("data/sword.obj", 2.0f, true);
    eng::MeshData lightMesh = eng::loadFromObj("data/light.obj", 0.1f, true, true);

    std::shared_ptr<eng::Texture> texture = std::make_shared<eng::Texture>("data/crate.jpg");
    std::shared_ptr<eng::Texture> texture2 = std::make_shared<eng::Texture>("data/texture2.png");
    std::shared_ptr<eng::Texture> texture3 = std::make_shared<eng::Texture>("data/texture3.png");
    std::shared_ptr<eng::Texture> textureStone = std::make_shared<eng::Texture>("data/textureStone.png");
    std::shared_ptr<eng::Texture> swordTexture = std::make_shared<eng::Texture>("data/sword.png");

    std::shared_ptr<eng::Texture> brickTexture = std::make_shared<eng::Texture>("data/brickwall.jpg");
    std::shared_ptr<eng::Texture> brickNormalMap = std::make_shared<eng::Texture>("data/brickwall_normal.jpg");

    std::shared_ptr<eng::CubemapTexture> skybox = std::make_shared<eng::CubemapTexture>("data/LancellottiChapel");

    sf::Font font;
    sf::Text text;
    sf::Text text2;
    sf::Text text3;
};
