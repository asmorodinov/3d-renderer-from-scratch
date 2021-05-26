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

class UserInterface {
 public:
    using Frames = std::size_t;
    using Seconds = float;
    using FramesPerSecond = float;

    UserInterface(sf::RenderWindow& window_);

    void updateAndDraw(Seconds deltaTime, size_t trianglesCount = 0);

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
};
