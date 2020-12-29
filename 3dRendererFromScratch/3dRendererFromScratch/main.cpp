#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "Engine/Renderer.h"

#include "profile.h"

int main() {
    size_t width = 800;
    size_t height = 600;

    sf::RenderWindow window(sf::VideoMode(width, height), "SFML window [press SPACE to toggle rendering modes]");
    eng::Renderer renderer(width, height);
    auto clr = renderer.getScreen().getClearColor();

    sf::Font font;
    if (!font.loadFromFile("data/arial.ttf")) {
        std::cout << "Failed to load font\n";
        return 1;
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);
    text.setOrigin({0.0f, -22.0f});
    sf::Text text2;
    text2.setFont(font);
    text2.setCharacterSize(18);
    text2.setOrigin({0.0f, -44.0f});
    sf::Text text3;
    text3.setFont(font);
    text3.setCharacterSize(18);
    text3.setOrigin({0.0f, 0.0f});

    eng::FlatShader fshader;
    fshader.setConst({glm::vec3(1.0f)});

    fshader.setShader([](const glm::vec3& color, const eng::FlatVar& var, const eng::LightsVec& lights) -> glm::vec4 {
        return glm::vec4(color, 1.0f);
    });

    eng::TextureShader tshader;
    tshader.setConst(eng::Texture("data/texture.png"));

    tshader.setShader([](const eng::Texture& fl, const glm::vec2& var, const eng::LightsVec& lights) -> glm::vec4 {
        return fl.sample(var.s, var.t);
    });

    eng::PhongShader pshader;
    pshader.setConst({eng::Texture("data/texture.png"), glm::vec3(), glm::vec3()});

    pshader.setShader([](const eng::PhongConst& ph, const eng::PhongVar& var, const eng::LightsVec& lights) -> glm::vec4 {
        glm::vec4 color = ph.t.sample(var.uv.s, var.uv.t);
        glm::vec3 color1 = color;

        glm::vec3 lighting = color1 * 0.00f;

        glm::vec3 FragPos = var.pos;
        glm::vec3 viewPos = ph.viewPos;
        glm::vec3 normal = ph.normal;

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

    eng::TextureShader tshader2 = tshader;
    tshader2.setConst(eng::Texture("data/sword.png"));

    eng::PhongShader pshader2 = pshader;
    pshader2.getConst().t = eng::Texture("data/sword.png");

    eng::TextureShader tshader3 = tshader;
    tshader3.setConst(eng::Texture("data/texture2.png"));

    eng::PhongShader pshader3 = pshader;
    pshader3.getConst().t = eng::Texture("data/texture2.png");

    eng::TextureShader tshader4 = tshader;
    tshader4.setConst(eng::Texture("data/texture3.png"));

    eng::PhongShader pshader4 = pshader;
    pshader4.getConst().t = eng::Texture("data/texture3.png");

    int frames = 0;
    float lastTime = 0.0f;
    float fps = 0.0;

    eng::RenderMode fl = eng::RenderMode::Phong;

    float time = 0.0f;
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) fl = static_cast<eng::RenderMode>((static_cast<int>(fl) + 1) % 3);
                if (event.key.code == sf::Keyboard::Escape) window.close();
            }
        }

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

        auto& objects = renderer.getWorld().getObjects();
        objects.clear();

        auto& cam = renderer.getWorld().getCamera();
        cam.setPosition(4.5f * glm::vec3(std::sin(glm::radians(t2 / 3.0f)), 0.0f, std::cos(glm::radians(t2 / 3.0f))) +
                        glm::vec3(0.0f, 0.8f + 0.8f * std::cos(glm::radians(t2 / 1.0f)), 0.0f));
        cam.setDirection(-cam.getPosition());

        auto& screen = renderer.getScreen();
        screen.setClearColor(clr * (2.0f + 0.2f * std::sin(glm::radians(t))));

        auto& lights = renderer.getWorld().getPointLights();
        if (lights.empty()) {
            lights.push_back(eng::PointLight({2, 0.1, 0}, {0.2, 1, 0.2}, 1.2f, 0.5f));
            lights.push_back(eng::PointLight({-2, 0.1, 0}, {1, 0.2, 0.2}, 1.2f, 0.5f));
            lights.push_back(eng::PointLight({0, 0.1, 2}, {0.2, 0.2, 1.0}, 1.2f, 0.5f));
            lights.push_back(eng::PointLight({0, 0.1, -2}, {0.2, 1.0, 1.0}, 1.2f, 0.5f));
        }

        float s = 2.6f;
        float h = -0.8;

        objects.emplace_back(std::make_unique<eng::Mesh>(eng::MeshData{{{-s, h, s}, {-s, h, -s}, {s, h, s}, {s, h, -s}},
                                                                       {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                                                       {{0, 1, 2, 0, 3, 1}, {3, 2, 1, 2, 1, 3}}},
                                                         eng::ColorType(1.0f), pshader, fshader, tshader, fl));

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
                                                         eng::ColorType(1.0f), pshader, fshader, tshader, fl));
        objects.back()->getTransform().position = glm::vec3(-1.0f, h + sz, 0.8f);

        objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/teapot.obj", 0.4f, true, false),
                                                         eng::ColorType(1.0f), pshader4, fshader, tshader4, fl));
        objects.back()->getTransform().position.y = h;

        objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/lowPolySphere.obj", 0.4f, true),
                                                         eng::ColorType(1.0f), pshader3, fshader, tshader3, fl));
        objects.back()->getTransform().position = glm::vec3(1.0f, h + 0.4f, -0.8f);

        objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/sword.obj", 2.0f, true), eng::ColorType(1.0f),
                                                         pshader2, fshader, tshader2, fl));
        objects.back()->getTransform().position = glm::vec3(-1.0f, h, -0.8f);

        for (int y = 0; y < lights.size(); ++y) {
            glm::vec3 pos = lights[y].pos;
            glm::vec3 color = lights[y].color;
            float scale = 0.1f;
            eng::RenderMode flat = eng::RenderMode::FlatColor;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(scale));

            glm::mat4 model2 = glm::mat4(1.0f);
            model2 = glm::translate(model2, pos);
            model2 = glm::scale(model2, -glm::vec3(scale));

            std::vector<glm::vec3> points = {{1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};
            for (auto& p : points) p = model * glm::vec4(p, 1.0f);

            std::vector<glm::vec3> points2 = {{1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};
            for (auto& p : points2) p = model2 * glm::vec4(p, 1.0f);

            for (int i = 0; i < 4; ++i)
                for (int j = i + 1; j < 4; ++j) {
                    objects.emplace_back(std::make_unique<eng::Line>(points[i], points[j], color));
                    objects.emplace_back(std::make_unique<eng::Line>(points2[i], points2[j], color));
                }
        }

        renderer.clearScreen();
        text3.setString("triangles: " + std::to_string(renderer.renderSceneToScreen()));

        window.clear();
        renderer.renderScreenToSFMLWindow(window);
        window.draw(text);
        window.draw(text2);
        window.draw(text3);
        window.display();

        // std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0f * (1.0f / fps - dt))));
    }

    return 0;
}
