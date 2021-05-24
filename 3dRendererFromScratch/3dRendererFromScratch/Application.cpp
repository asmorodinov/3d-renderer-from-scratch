#include "Application.h"

Application::Application()
    : width(default_width),
      height(default_height),
      window(sf::VideoMode(width, height), mainWindowMsg_),
      ui(window),
      renderer(width, height, window),
      scene(eng::makeScene1()) {}

void Application::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) window.close();

            keyPressedOrReleased(event.key.code, true);
        } else if (event.type == sf::Event::KeyReleased) {
            keyPressedOrReleased(event.key.code, false);
        } else if (event.type == sf::Event::MouseMoved) {
            mouseMove(event.mouseMove.x, event.mouseMove.y);
            int x = width / 2;
            int y = height / 2;
            sf::Mouse::setPosition(sf::Vector2i(x, y), window);
            mouseMove(-1.0f, -1.0f);
            mouseMove(x, y);

        } else if (event.type == sf::Event::MouseEntered) {
            mouseMove(-1.0f, -1.0f);
        }
    }
}

void Application::run() {
    sf::Clock deltaClock;
    while (window.isOpen()) {
        processEvents();

        float dt = deltaClock.restart().asSeconds();

        update(dt);

        window.clear();

        size_t trianglesDrawn = renderer.render(scene);
        ui.updateAndDraw(dt, trianglesDrawn);

        window.display();
    }
}

void Application::mouseMove(float x, float y) {
    if (mx >= 0.0f && my >= 0.0f && x >= 0.0f && y >= 0.0f) {
        cameraControl.mouseMove(x - mx, y - my);
    }
    mx = x;
    my = y;
}

void Application::keyPressedOrReleased(sf::Keyboard::Key key, bool mode) { cameraControl.keyPressedOrReleased(key, mode); }

void Application::update(float dt) {
    cameraControl.update(dt);

    scene.getCamera().setPosition(cameraControl.position);
    scene.getCamera().setDirection(cameraControl.direction);
}
