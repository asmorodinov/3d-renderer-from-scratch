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
    using Pixels = eng::Screen::Pixels;
    using Seconds = float;

    static constexpr const Pixels defaultWidth_ = 1200;
    static constexpr const Pixels defaultHeight_ = 800;
    static constexpr const char* mainWindowMsg_ = "3d renderer";

 public:
    Application();

    void run();

 private:
    void processEvents();
    void mouseMove(glm::vec2 newPosition);
    void keyPressedOrReleased(sf::Keyboard::Key key, bool mode);

    void update(Seconds dt);

    sf::RenderWindow mainAppWindow_;
    UserInterface userInterface_;

    SFMLRenderer renderer_;
    eng::Scene scene_;

    eng::CameraControl cameraControl_;
    glm::vec2 mouseCoords_ = {-1.0f, -1.0f};
};
