#pragma once

#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstddef>

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <imgui-SFML.h>

#include "Engine/Assets.h"
#include "Engine/Renderer.h"
#include "Engine/SampleScenes.h"

#include "Engine/MeshIO.h"

class UserInterface {
 public:
    using Frames = std::size_t;
    using Seconds = float;
    using FramesPerSecond = float;

    UserInterface(sf::RenderWindow& window_);

    void updateAndDraw(Seconds deltaTime, size_t trianglesCount, size_t& sceneIndex, std::vector<eng::Scene>& scenes);

    void togglePause();

 private:
    std::reference_wrapper<sf::RenderWindow> mainAppWindow_;

    Frames frames_ = 0;
    Seconds currentTime_ = 0.0f;
    Seconds lastTime_ = 0.0f;

    FramesPerSecond framesPerSecond_ = 0.0f;

    sf::Font font_;

    sf::Text text_;
    sf::Text text2_;
    sf::Text text3_;
    sf::Text text4_;
    sf::Text text5_;

    bool pause = false;
};
