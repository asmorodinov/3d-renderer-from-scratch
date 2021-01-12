#include "Application.h"

Application::Application()
    : width(800),
      height(600),
      window(sf::VideoMode(width, height), "SFML window [press SPACE to toggle rendering modes]"),
      renderer(width, height),
      rm(eng::RenderMode::Normals) {
    initInterface();
    initShaders();
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

void Application::initShaders() {
    flatShader.setConst({glm::vec3(1.0f)});

    flatShader.setShader([](const eng::ShaderConstantsVec& consts, const eng::ShaderVariablesVec& var,
                         const eng::LightsVec& lights) -> glm::vec4 { return glm::vec4(consts[0].get<glm::vec3>(), 1.0f); });

    textureShader.setConst({&texture});

    textureShader.setShader(
        [](const eng::ShaderConstantsVec& consts, const eng::ShaderVariablesVec& var, const eng::LightsVec& lights) -> glm::vec4 {
            auto uv = var[0].get<glm::vec2>();
            return consts[0].get<eng::Texture*>()->sample(uv.s, uv.t);
        });

    uvShader.setShader(
        [](const eng::ShaderConstantsVec& consts, const eng::ShaderVariablesVec& var, const eng::LightsVec& lights) -> glm::vec4 {
            auto uv = var[0].get<glm::vec2>();
            return glm::vec4(uv.s, uv.t, 0.5f, 1.0f);
        });

    normalShader.setShader(
        [](const eng::ShaderConstantsVec& consts, const eng::ShaderVariablesVec& var, const eng::LightsVec& lights) -> glm::vec4 {
            auto normal = consts[0].get<glm::vec3>();
            return glm::vec4(0.5f * (normal + 1.0f), 1.0f);
        });

    phongShader.setConst({&texture, glm::vec3(), glm::vec3()});

    phongShader.setShader([](const eng::ShaderConstantsVec& consts, const eng::ShaderVariablesVec& var,
                         const eng::LightsVec& lights) -> glm::vec4 {
        auto FragPos = var[0].get<glm::vec3>();
        auto uv = var[1].get<glm::vec2>();

        auto texture = consts[0].get<eng::Texture*>();
        auto viewPos = consts[1].get<glm::vec3>();
        auto normal = consts[2].get<glm::vec3>();

        glm::vec4 color = texture->sample(uv.s, uv.t);
        glm::vec3 color1 = color;

        glm::vec3 lighting = color1 * 0.00f;

        for (const auto& light : lights) {
            glm::vec3 lightPos = light.pos;
            glm::vec3 viewDir = glm::normalize(viewPos - FragPos);
            glm::vec3 lightDir = glm::normalize(lightPos - FragPos);

            float diff = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.2f);
            glm::vec3 diffuse = diff * color1 * light.color * light.diff;

            glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
            float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
            glm::vec3 specular = light.color * spec * light.spec;

            float d = length(lightPos - FragPos);
            float attenuation = 1.0f / (1.0f + d * light.lin + std::pow(d, 2.0f) * light.quad + std::pow(d, 3.0f) * light.cube);
            diffuse *= attenuation / 1.7;
            specular *= attenuation / 1.7;
            lighting += light.intensity * glm::vec3(1.0f) * (diffuse + specular);
        }

        lighting = glm::pow(lighting, glm::vec3(1.0f / 2.2f));

        return glm::vec4(lighting, color.a);
    });
}

void Application::addObjects() {
    auto& objects = renderer.getWorld().getObjects();

    auto& lights = renderer.getWorld().getPointLights();
    if (lights.empty()) {
        lights.push_back(eng::PointLight({2, 0.1, 0}, {0.2, 1, 0.2}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({-2, 0.1, 0}, {1, 0.2, 0.2}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({0, 0.1, 2}, {0.2, 0.2, 1.0}, 1.2f, 0.5f));
        lights.push_back(eng::PointLight({0, 0.1, -2}, {0.2, 1.0, 1.0}, 1.2f, 0.5f));
    }

    // plane
    float s = 2.6f;
    float h = -0.8;

    objects.emplace_back(std::make_unique<eng::Mesh>(eng::MeshData{{{-s, h, s}, {-s, h, -s}, {s, h, s}, {s, h, -s}},
                                                                   {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                                                   {{0, 1, 2, 0, 3, 1}, {3, 2, 1, 2, 1, 3}}},
                                                     &texture, eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader,
                                                     normalShader));

    // cube
    float sz = 0.3f;
    objects.emplace_back(std::make_unique<eng::Mesh>(eng::MeshData{{{-sz, -sz, -sz},
                                                                    {-sz, -sz, sz},
                                                                    {-sz, sz, -sz},
                                                                    {-sz, sz, sz},
                                                                    {sz, -sz, -sz},
                                                                    {sz, -sz, sz},
                                                                    {sz, sz, -sz},
                                                                    {sz, sz, sz}},
                                                                   {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                                                   {{1, 3, 7, 0, 1, 2},
                                                                    {1, 7, 5, 0, 2, 3},
                                                                    {5, 7, 6, 0, 1, 2},
                                                                    {5, 6, 4, 0, 2, 3},
                                                                    {4, 6, 2, 0, 1, 2},
                                                                    {4, 2, 0, 0, 2, 3},
                                                                    {0, 2, 3, 0, 1, 2},
                                                                    {0, 3, 1, 0, 2, 3},
                                                                    {3, 2, 6, 0, 1, 2},
                                                                    {3, 6, 7, 0, 2, 3},
                                                                    {0, 1, 5, 0, 1, 2},
                                                                    {0, 5, 4, 0, 2, 3}}},
                                                     &texture, eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader,
                                                     normalShader));
    objects.back()->getTransform().position = glm::vec3(-1.0f, h + sz, 0.8f);

    // teapot
    objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/teapot.obj", 0.4f, true, false), &texture3,
                                                     eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader));
    objects.back()->getTransform().position.y = h;

    // sphere
    for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 3; ++x) {
            objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/lowPolySphere.obj", 0.4f, true), &texture2,
                                                             eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader,
                                                             normalShader));
            objects.back()->getTransform().position = glm::vec3(1.0f + 0.2f * x, h + 0.4f + 0.2f * y, -0.8f);
        }

    // sword
    objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/sword.obj", 2.0f, true), &swordTexture,
                                                     eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader));
    objects.back()->getTransform().position = glm::vec3(-1.0f, h, -0.8f);

    eng::Mesh light(eng::loadFromObj("data/light.obj", 0.1f, true, false), &texture, eng::ColorType(1.0f), phongShader, flatShader,
                    textureShader, uvShader, normalShader, eng::RenderMode::FlatColor);

    for (int y = 0; y < lights.size(); ++y) {
        glm::vec3 pos = lights[y].pos;
        glm::vec3 color = lights[y].color;

        auto lightObj = std::make_unique<eng::Mesh>(light);
        lightObj->setPosition(pos);
        lightObj->setColor(color);

        objects.emplace_back(std::move(lightObj));
    }
}

void Application::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) rm = static_cast<eng::RenderMode>((static_cast<int>(rm) + 1) % 6);
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

        auto& cam = renderer.getWorld().getCamera();
        cam.setPosition(4.5f * glm::vec3(std::sin(glm::radians(t2 / 3.0f)), 0.0f, std::cos(glm::radians(t2 / 3.0f))) +
                        glm::vec3(0.0f, 0.8f + 0.8f * std::cos(glm::radians(t2 / 1.0f)), 0.0f));
        cam.setDirection(-cam.getPosition());

        renderer.clearScreen();
        renderer.setRenderMode(rm);

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