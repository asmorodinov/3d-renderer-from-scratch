#include "Application.h"

Application::Application() : width(default_width), height(default_height), window(sf::VideoMode(width, height), mainWindowMsg_), renderer(width, height) {
    initInterface();
    addObjects();
}

void Application::initInterface() {
    window.setMouseCursorVisible(false);

    if (!font.loadFromFile("data/fonts/arial.ttf")) {
        std::cout << "Failed to load font\n";
        return;
    }

    text.setFont(font);
    text.setCharacterSize(18);
    text.setOrigin({0.0f, -22.0f});

    text2.setFont(font);
    text2.setCharacterSize(18);
    text2.setOrigin({0.0f, -44.0f});

    text3.setFont(font);
    text3.setCharacterSize(18);
    text3.setOrigin({0.0f, 0.0f});
}

void Application::addObjects() {
    auto& scene = renderer.getScene();

    scene = eng::makeScene1();
}

void Application::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) window.close();

            renderer.keyPressedOrReleased(event.key.code, true);
        } else if (event.type == sf::Event::KeyReleased) {
            renderer.keyPressedOrReleased(event.key.code, false);
        } else if (event.type == sf::Event::MouseMoved) {
            renderer.mouseMove(event.mouseMove.x, event.mouseMove.y);
            int x = width / 2;
            int y = height / 2;
            sf::Mouse::setPosition(sf::Vector2i(x, y), window);
            renderer.mouseMove(-1.0f, -1.0f);
            renderer.mouseMove(x, y);

        } else if (event.type == sf::Event::MouseEntered) {
            renderer.mouseMove(-1.0f, -1.0f);
        }
    }
}

void Application::run() {
    float time = 0.0f;
    sf::Clock deltaClock;
    while (window.isOpen()) {
        processEvents();

        float dt = deltaClock.restart().asSeconds();
        time += dt;
        ++frames;

        if (time - lastTime >= 1.5f) {
            fps = frames / (time - lastTime);
            text.setString("FPS: " + std::to_string(static_cast<int>(fps)));
            text2.setString("ms per frame: " + std::to_string(static_cast<int>(1000.0f / fps)));
            frames = 0;
            lastTime = time;
        }

        float t = 0.2f * 360.0f / 20.0f * time;
        float t2 = t * 5.0f;

        auto& cam = renderer.getScene().getCamera();
        cam.setPosition(4.5f * glm::vec3(std::sin(glm::radians(t2 / 3.0f)), 0.0f, std::cos(glm::radians(t2 / 3.0f))) +
                        glm::vec3(0.0f, 0.8f + 0.8f * std::cos(glm::radians(t2 / 1.0f)), 0.0f));
        cam.setDirection(-cam.getPosition());

        renderer.clearScreen();

        renderer.update(dt);
        text3.setString("triangles: " + std::to_string(renderer.renderSceneToScreen()));

        window.clear();
        renderer.renderScreenToSFMLWindow(window);
        window.draw(text);
        window.draw(text2);
        window.draw(text3);

        window.display();
    }
}
