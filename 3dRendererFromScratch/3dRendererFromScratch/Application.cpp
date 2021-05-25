#include "Application.h"

Application::Application()
    : mainAppWindow_(sf::VideoMode(defaultWidth_, defaultHeight_), mainWindowMsg_),
      userInterface_(mainAppWindow_),
      renderer_(defaultWidth_, defaultHeight_, mainAppWindow_),
      scene_(eng::makeScene1()) {}

void Application::processEvents() {
    sf::Event event;
    while (mainAppWindow_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            mainAppWindow_.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) mainAppWindow_.close();

            keyPressedOrReleased(event.key.code, true);
        } else if (event.type == sf::Event::KeyReleased) {
            keyPressedOrReleased(event.key.code, false);
        } else if (event.type == sf::Event::MouseMoved) {
            mouseMove({event.mouseMove.x, event.mouseMove.y});
            int x = defaultWidth_ / 2;
            int y = defaultHeight_ / 2;
            sf::Mouse::setPosition(sf::Vector2i(x, y), mainAppWindow_);
            mouseMove({-1.0f, -1.0f});
            mouseMove({x, y});

        } else if (event.type == sf::Event::MouseEntered) {
            mouseMove({-1.0f, -1.0f});
        }
    }
}

void Application::run() {
    sf::Clock deltaClock;
    while (mainAppWindow_.isOpen()) {
        processEvents();

        float dt = deltaClock.restart().asSeconds();

        update(dt);

        mainAppWindow_.clear();

        size_t trianglesDrawn = renderer_.render(scene_);
        userInterface_.updateAndDraw(dt, trianglesDrawn);

        mainAppWindow_.display();
    }
}

void Application::mouseMove(glm::vec2 newPosition) {
    if (mouseCoords_.x >= 0.0f && mouseCoords_.y >= 0.0f && newPosition.x >= 0.0f && newPosition.y >= 0.0f) {
        cameraControl_.mouseMove(newPosition - mouseCoords_);
    }
    mouseCoords_ = newPosition;
}

void Application::keyPressedOrReleased(sf::Keyboard::Key key, bool mode) { cameraControl_.keyPressedOrReleased(key, mode); }

void Application::update(Seconds dt) {
    cameraControl_.update(dt);

    scene_.getCamera().setPosition(cameraControl_.getPosition());
    scene_.getCamera().setDirection(cameraControl_.getDirection());
}
