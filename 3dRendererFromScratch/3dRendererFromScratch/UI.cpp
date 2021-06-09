#include "UI.h"

#include <cstdlib>

UserInterface::UserInterface(sf::RenderWindow& window_) : mainAppWindow_(window_) {
    if (!font_.loadFromFile("data/fonts/arial.ttf")) {
        std::cout << "Failed to load font\n";
        exit(1);
    }

    ImGui::SFML::Init(mainAppWindow_);

    text_.setFont(font_);
    text_.setCharacterSize(18);
    text_.setOrigin({0.0f, -22.0f});

    text2_.setFont(font_);
    text2_.setCharacterSize(18);
    text2_.setOrigin({0.0f, -44.0f});

    text3_.setFont(font_);
    text3_.setCharacterSize(18);
    text3_.setOrigin({0.0f, 0.0f});
}

void UserInterface::updateAndDraw(Seconds deltaTime, size_t trianglesCount) {
    currentTime_ += deltaTime;
    ++frames_;

    if (currentTime_ - lastTime_ >= 1.5f) {
        framesPerSecond_ = frames_ / (currentTime_ - lastTime_);
        text_.setString("FPS: " + std::to_string(static_cast<int>(framesPerSecond_)));
        text2_.setString("ms per frame: " + std::to_string(static_cast<int>(1000.0f / framesPerSecond_)));
        frames_ = 0;
        lastTime_ = currentTime_;
    }

    text3_.setString("triangles: " + std::to_string(trianglesCount));

    mainAppWindow_.get().draw(text_);
    mainAppWindow_.get().draw(text2_);
    mainAppWindow_.get().draw(text3_);

    ImGui::SFML::Update(mainAppWindow_, sf::seconds(deltaTime));

    if (pause) {
        ImGui::ShowDemoWindow();
    } else {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    }

    ImGui::SFML::Render(mainAppWindow_);
}

void UserInterface::togglePause() {
    pause = !pause;
}
