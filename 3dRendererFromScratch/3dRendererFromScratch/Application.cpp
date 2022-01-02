#include "Application.h"

Application::Application()
    : mainAppWindow_(sf::VideoMode(defaultWidth_, defaultHeight_), mainWindowMsg_),
      userInterface_(mainAppWindow_),
      renderer_(defaultWidth_, defaultHeight_, mainAppWindow_),
      scenes_{eng::loadSceneFromFile("scene1.scn"), eng::loadSceneFromFile("scene2.scn")},
      currentScene_(scenes_.front()) {
}

void Application::run() {
    sf::Clock deltaClock;
    while (mainAppWindow_.isOpen()) {
        processEvents();

        float dt = deltaClock.restart().asSeconds();

        update(dt);

        mainAppWindow_.clear();

        size_t trianglesDrawn = renderer_.render(currentScene_);
        userInterface_.updateAndDraw(dt, trianglesDrawn, currentSceneIndex_, scenes_);
        currentScene_ = scenes_[currentSceneIndex_];

        mainAppWindow_.display();
    }
}

void Application::processEvents() {
    sf::Event event;
    while (mainAppWindow_.pollEvent(event)) {
        ImGui::SFML::ProcessEvent(event);

        switch (event.type) {
            case sf::Event::Closed:
                onClose();
                break;
            case sf::Event::KeyPressed:
                onKeyPressOrRelease(event.key.code, true);
                break;
            case sf::Event::KeyReleased:
                onKeyPressOrRelease(event.key.code, false);
                break;
            case sf::Event::MouseMoved:
                onMouseMove({event.mouseMove.x, event.mouseMove.y});
                break;
            case sf::Event::MouseEntered:
                onMouseEnter();
                break;
            default:
                break;
        }
    }
}

void Application::onClose() {
    mainAppWindow_.close();
    ImGui::SFML::Shutdown();
}

void Application::onMouseEnter() {
    mouseCoordinates_ = glm::vec2(-1.0f, -1.0f);
}

void Application::onMouseMove(glm::vec2 newPosition) {
    if (!pause) {
        if (mouseCoordinates_.x >= 0.0f && mouseCoordinates_.y >= 0.0f) {
            cameraControl_.mouseMove(newPosition - mouseCoordinates_);
        }

        Pixels x = defaultWidth_ / 2;
        Pixels y = defaultHeight_ / 2;

        sf::Mouse::setPosition(sf::Vector2i(x, y), mainAppWindow_);

        mouseCoordinates_ = glm::vec2(x, y);
    } else {
        mouseCoordinates_ = newPosition;
    }
}

void Application::onKeyPressOrRelease(sf::Keyboard::Key key, bool mode) {
    if (mode && key == sf::Keyboard::Escape) mainAppWindow_.close();

    if (mode && key == sf::Keyboard::Tab) {
        userInterface_.togglePause();
        pause = !pause;
    }
    if (mode && key == sf::Keyboard::LShift) {
        currentSceneIndex_ = (currentSceneIndex_ + 1) % scenes_.size();
        currentScene_ = scenes_[currentSceneIndex_];
    }

    if (!pause) cameraControl_.keyPressedOrReleased(key, mode);
}

void Application::update(Seconds dt) {
    cameraControl_.update(dt);

    currentScene_.get().getCamera().setPosition(cameraControl_.getPosition());
    currentScene_.get().getCamera().setDirection(cameraControl_.getDirection());
}
