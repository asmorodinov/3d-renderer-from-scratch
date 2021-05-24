#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include "Engine/CameraControl.h"
#include "Engine/SampleScenes.h"
#include "UI.h"
#include "SFMLRenderer.h"

class Application {
 private:
    using Pixels = std::size_t;
    static constexpr const Pixels default_width = 1200;
    static constexpr const Pixels default_height = 800;
    static constexpr const char* mainWindowMsg_ = "SFML window [press SPACE to toggle rendering modes]";

 public:
    Application();

    void run();

 private:
    void processEvents();
    void mouseMove(float x, float y);
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode);

    void update(float dt);

    size_t width;
    size_t height;

    sf::RenderWindow window;
    UI ui;

    SFMLRenderer renderer;
    eng::Scene scene;

    eng::CameraControl cameraControl;
    float mx = -1.0f, my = -1.0f;
};
