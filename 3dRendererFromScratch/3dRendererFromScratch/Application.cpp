#include "Application.h"

Application::Application()
    : width(default_width),
      height(default_height),
      window(sf::VideoMode(width, height), mainWindowMsg_),
      renderer(width, height) {
    initInterface();
    addObjects();
}

void Application::initInterface() {
    window.setMouseCursorVisible(false);

    if (!font.loadFromFile("data/arial.ttf")) {
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
    auto& objects = renderer.getScene().getObjects();

    auto& lights = renderer.getScene().getPointLights();
    if (lights.empty()) {
        lights.push_back(eng::PointLight({2, 0.1, 0}, {0.2, 1, 0.2}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({-2, 0.1, 0}, {1, 0.2, 0.2}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({0, 0.1, 2}, {0.2, 0.2, 1.0}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({0, 0.1, -2}, {0.2, 1.0, 1.0}, 1.2f, 0.5f));
    }

    // objects.cubemapMeshes.push_back(
    //    eng::CubemapMesh(std::cref(skyboxMesh), {}, eng::Assets::getCubemapTexture("LancellottiChapel"), {}, false, false));

    // plane
    objects.textureMeshes.push_back(eng::TextureMesh(std::cref(planeMesh), {}, eng::Assets::getTexture("textureStone.png"), {}));

    // cube

    objects.textureMeshes.push_back(eng::TextureMesh(std::cref(cubeMesh), {}, eng::Assets::getTexture("crate.jpg"), {}));

    objects.textureMeshes.back().getTransform().position = glm::vec3(-1.0f, h + sz, 0.8f);

    // cube 2
    objects.textureMeshes.push_back(eng::TextureMesh(std::cref(cubeMesh), {}, eng::Assets::getTexture("brickwall.jpg"), {}));
    objects.textureMeshes.back().getTransform().position = glm::vec3(-1.0f - 2.5f * sz, h + sz, 0.8f);

    // teapot
    objects.flatMeshes.push_back(
        eng::FlatMesh(eng::Assets::getMeshData("teapot", 0.4f, false, true), {}, glm::vec3(1.0f, 1.0f, 1.0f), {}));
    objects.flatMeshes.back().getTransform().position.y = h;

    /*
    // sphere
    for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 3; ++x) {
            objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/lowPolySphere.obj", 0.4f, true), texture2,
    eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader, normalMapShader));
            objects.back()->getTransform().position = glm::vec3(1.0f + 0.6f * x, h + 0.4f + 0.6f * y, -0.8f);
        }

    // sword
    objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/sword.obj", 2.0f, true), swordTexture,
    eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader, normalMapShader));
    objects.back()->getTransform().position = glm::vec3(-1.0f, h, -0.8f);

    eng::Mesh light(eng::loadFromObj("data/light.obj", 0.1f, true, true), texture, eng::ColorType(1.0f), phongShader, flatShader,
    textureShader, uvShader, normalShader, normalMapShader, eng::RenderMode::FlatColor);

    for (int y = 0; y < lights.size(); ++y) {
        glm::vec3 pos = lights[y].pos;
        glm::vec3 color = lights[y].color;

        auto lightObj = std::make_unique<eng::Mesh>(light);
        lightObj->setPosition(pos);
        lightObj->setColor(color);

        objects.emplace_back(std::move(lightObj));
    }
    */
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
