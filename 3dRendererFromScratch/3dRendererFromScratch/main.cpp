#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "Engine/Renderer.h"

#include "profile.h"

int main() {
    size_t width = 1200;
    size_t height = 800;

    sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
    eng::Renderer renderer(width, height);
    auto clr = renderer.getScreen().getClearColor();

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Failed to load font\n";
        return 1;
    }
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18);

    eng::FlatShader fshader;
    fshader.getConst().color = glm::vec3(1.0f);

    fshader.getShader() = [](const eng::FlatConst& fl, const eng::FlatVar& var, const eng::LightsVec& lights) -> glm::vec3 {
        return fl.color;
    };

    eng::PhongShader pshader;
    pshader.getConst().t = eng::Texture("texture.png");

    pshader.getShader() = [](const eng::PhongConst& ph, const eng::PhongVar& var, const eng::LightsVec& lights) -> glm::vec3 {
        glm::vec3 color1 = ph.t.sample(var.uv.s, var.uv.t);

        glm::vec3 lighting = color1 * 0.00f;

        glm::vec3 FragPos = var.pos;
        glm::vec3 viewPos = ph.viewPos;
        glm::vec3 normal = ph.normal;

        for (auto& light : lights) {
            glm::vec3 lightPos = light.pos;
            glm::vec3 viewDir = glm::normalize(viewPos - FragPos);
            glm::vec3 lightDir = glm::normalize(lightPos - FragPos);

            glm::vec3 diffuse = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.2f) * color1 * light.color * light.diff;

            glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
            float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
            glm::vec3 specular = light.color * spec * light.spec;

            float d = length(lightPos - FragPos);
            float attenuation = 1.0 / (1.0 + d * light.lin + std::pow(d, 2) * light.quad + std::pow(d, 3) * light.cube);
            diffuse *= attenuation / 1.7;
            specular *= attenuation / 1.7;
            lighting += light.intensity * glm::vec3(1.0f) * (diffuse + specular);
        }

        lighting = glm::pow(lighting, glm::vec3(1.0f / 2.2f));

        return lighting;
    };

    int frames = 0;
    float lastTime = 0.0f;
    int fps = 0;

    float time = 0.0f;
    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        float dt = deltaClock.restart().asSeconds();
        time += dt;
        ++frames;

        if (frames == 100) {
            fps = int(frames / (time - lastTime));
            text.setString("FPS: " + std::to_string(fps));
            frames = 0;
            lastTime = time;
        }

        float t = 0.4f * 360.0f / 20.0f * time;
        float t2 = t * 5.0f;

        auto& objects = renderer.getWorld().getObjects();
        objects.clear();

        auto& cam = renderer.getWorld().getCamera();
        cam.setPosition(5.5f * glm::vec3(std::cos(glm::radians(t2 / 3.0f)), 0.0f, std::sin(glm::radians(t2 / 3.0f))) +
                        glm::vec3(0.0f, 1.5f + 0.8f * std::cos(glm::radians(t2 / 1.0f)), 0.0f));
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

        float s = 1.6f;
        float h = -0.8;
        objects.emplace_back(std::make_unique<eng::TriangleObj>(glm::vec3(-s, h, s), glm::vec3(-s, h, -s), glm::vec3(s, h, s),
                                                                eng::ColorType(1.0f), pshader, fshader));
        objects.emplace_back(std::make_unique<eng::TriangleObj>(glm::vec3(s, h, -s), glm::vec3(s, h, s), glm::vec3(-s, h, -s),
                                                                eng::ColorType(1.0f), pshader, fshader, true));

        float sc = 1.1f;
        for (int z = -1; z <= 1; ++z)
            for (int x = -1; x <= 1; ++x)
                for (int y = 0; y <= lights.size(); ++y) {
                    glm::vec3 ax = glm::vec3(1.0f * (x + 2), 0.8f * (y + 2), 0.7f * (z + 2));
                    glm::vec3 pos = sc * glm::vec3(x, y, z);
                    eng::ColorType color = {1, 1, 1};
                    float scale = 0.35f;
                    bool flat = false;

                    if (x == 0 && z == 0 && y > 0) {
                        pos = lights[y - 1].pos;
                        color = lights[y - 1].color;
                        scale = 0.1f;
                        flat = true;
                    }

                    if (y > 0 && (x != 0 || z != 0)) continue;

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, pos);
                    model = glm::rotate(model, glm::radians(t), ax);
                    model = glm::scale(model, glm::vec3(scale));

                    glm::mat4 model2 = glm::mat4(1.0f);
                    model2 = glm::translate(model2, pos);
                    model2 = glm::rotate(model2, glm::radians(t), ax);
                    model2 = glm::scale(model2, -glm::vec3(scale));

                    std::vector<glm::vec3> points = {{1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};
                    for (auto& p : points) p = model * glm::vec4(p, 1.0f);

                    std::vector<glm::vec3> points2 = {{1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};
                    for (auto& p : points2) p = model2 * glm::vec4(p, 1.0f);

                    for (auto [i, j, k] : std::vector<std::array<int, 3>>{{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}}) {
                        objects.emplace_back(std::make_unique<eng::TriangleObj>(points[j], points[i], points[k], color, pshader,
                                                                                fshader, false, flat));
                    }

                    for (auto [i, j, k] : std::vector<std::array<int, 3>>{{2, 0, 1}, {1, 0, 3}, {3, 0, 2}, {2, 1, 3}}) {
                        objects.emplace_back(std::make_unique<eng::TriangleObj>(points2[j], points2[i], points2[k], color,
                                                                                pshader, fshader, false, flat));
                    }
                }

        renderer.clearScreen();
        renderer.renderSceneToScreen();

        window.clear();
        renderer.renderScreenToSFMLWindow(window);
        window.draw(text);
        window.display();

        // std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0f * (1.0f / fps - dt))));
    }

    return 0;
}
