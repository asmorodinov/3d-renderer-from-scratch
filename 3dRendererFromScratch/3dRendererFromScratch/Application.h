#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include <filesystem>
namespace fs = std::filesystem;

#include "Engine/CameraControl.h"
#include "Engine/SampleScenes.h"
#include "Engine/MeshIO.h"
#include "UI.h"
#include "SFMLRenderer.h"
#include "Engine/Types.h"

class Application {
 private:
    using Pixels = eng::Pixels;
    using Seconds = float;

    static constexpr const Pixels defaultWidth_ = 1200;
    static constexpr const Pixels defaultHeight_ = 800;
    static constexpr const char* mainWindowMsg_ = "3d renderer";

 public:
    Application();

    void run();

 private:
    void processEvents();

    void onClose();
    void onMouseEnter();
    void onMouseMove(glm::vec2 newPosition);
    void onKeyPressOrRelease(sf::Keyboard::Key key, bool mode);

    void update(Seconds dt);

    sf::RenderWindow mainAppWindow_;
    UserInterface userInterface_;

    SFMLRenderer renderer_;
    std::vector<eng::Scene> scenes_;
    size_t currentSceneIndex_ = 0;

    eng::CameraControl cameraControl_;
    glm::vec2 mouseCoordinates_ = {-1.0f, -1.0f};

    bool pause = false;
};
