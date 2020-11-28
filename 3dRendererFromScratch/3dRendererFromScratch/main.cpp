#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "Engine/Renderer.h"

#include "profile.h"

int main() {
    size_t width = 980;
    size_t height = 680;

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

        float t = 360.0f / 10.0f * time;

        auto& objects = renderer.getWorld().getObjects();
        objects.clear();

        auto& cam = renderer.getWorld().getCamera();
        cam.setPosition(5.5f * glm::vec3(std::cos(glm::radians(t / 3.0f)), 0.0f, std::sin(glm::radians(t / 3.0f))) +
                        glm::vec3(0.0f, 1.5f + 0.8f * std::cos(glm::radians(t / 1.0f)), 0.0f));
        cam.setDirection(-cam.getPosition());

        auto& screen = renderer.getScreen();
        screen.setClearColor(clr * (2.0f + 0.2f * std::sin(glm::radians(t))));

        float s = 1.6f;
        float h = -0.8;
        objects.emplace_back(
            std::make_unique<eng::Triangle>(glm::vec3(-s, h, s), glm::vec3(-s, h, -s), glm::vec3(s, h, s), eng::ColorType(1.0f)));
        objects.emplace_back(std::make_unique<eng::Triangle>(glm::vec3(s, h, -s), glm::vec3(s, h, s), glm::vec3(-s, h, -s),
                                                             eng::ColorType(1.0f), true));

        float sc = 1.1f;
        for (int z = -1; z <= 1; ++z)
            for (int x = -1; x <= 1; ++x)
                for (int y = 0; y <= 0; ++y) {
                    glm::vec3 ax = glm::vec3(1.0f * (x + 2), 0.8f * (y + 2), 0.7f * (z + 2));
                    glm::vec3 pos = sc * glm::vec3(x, y, z);
                    float scale = 0.35f;

                    if (z == 0 && x == 0) {
                        pos = {2, 1, 0};
                        scale = 0.05f;
                    }

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
                        objects.emplace_back(std::make_unique<eng::Triangle>(points[j], points[i], points[k],
                                                                             eng::ColorType{i / 3.0f, j / 3.0f, k / 3.0f}));
                    }

                    for (auto [i, j, k] : std::vector<std::array<int, 3>>{{2, 0, 1}, {1, 0, 3}, {3, 0, 2}, {2, 1, 3}}) {
                        objects.emplace_back(std::make_unique<eng::Triangle>(points2[j], points2[i], points2[k],
                                                                             eng::ColorType{i / 3.0f, j / 3.0f, k / 3.0f}));
                    }
                }

        renderer.clearScreen();
        renderer.renderSceneToScreen();

        window.clear();
        renderer.renderScreenToSFMLWindow(window);
        window.draw(text);
        window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0f * (1.0f / fps - dt))));
    }

    return 0;
}
