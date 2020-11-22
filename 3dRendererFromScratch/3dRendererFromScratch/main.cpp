#include <array>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "Engine/Renderer.h"

#include "profile.h"

int main() {
    size_t width = 1080;
    size_t height = 640;

    sf::RenderWindow window(sf::VideoMode(width, height), "SFML window");
    eng::Renderer renderer(width, height);

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
        cam.setPosition(4.5f * glm::vec3(std::cos(glm::radians(t / 3.0f)), 0.0f, std::sin(glm::radians(t / 3.0f))) +
                        glm::vec3(0.0f, std::cos(glm::radians(t / 1.0f)), 0.0f));
        cam.setDirection(-cam.getPosition());

        float sc = 0.8f;
        for (int z = -1; z <= 1; ++z)
            for (int x = -1; x <= 1; ++x)
                for (int y = -1; y <= 1; ++y) {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, sc * glm::vec3(x, y, z));
                    model = glm::rotate(model, glm::radians(t), glm::vec3(1.0f * (x + 2), 0.8f * (y + 2), 0.7f * (z + 2)));
                    model = glm::scale(model, glm::vec3(0.25f));

                    glm::mat4 model2 = glm::mat4(1.0f);
                    model2 = glm::translate(model2, sc * glm::vec3(x, y, z));
                    model2 = glm::rotate(model2, glm::radians(t), glm::vec3(1.0f * (x + 2), 0.8f * (y + 2), 0.7f * (z + 2)));
                    model2 = glm::scale(model2, -glm::vec3(0.25f));

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
