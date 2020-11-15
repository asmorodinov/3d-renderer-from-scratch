#include <array>
#include <string>
#include <vector>

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
            text.setString("FPS: " + std::to_string(int(frames / (time - lastTime))));
            frames = 0;
            lastTime = time;
        }

        float t = 360.0f / 10.0f * time;

        renderer.getWorld().getObjects().clear();

        for (int x = -1; x <= 1; ++x)
            for (int y = -1; y <= 1; ++y) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(1.5f * x, 1.5f * y, -6.0));
                model = glm::rotate(model, glm::radians(t), glm::vec3(1.0f, 0.8f, -0.7f));
                model = glm::scale(model, glm::vec3(0.4f));

                std::vector<glm::vec3> points = {{1, 1, 1}, {-1, -1, 1}, {-1, 1, -1}, {1, -1, -1}};
                for (auto& p : points) p = model * glm::vec4(p, 1.0f);

                for (auto [i, j, k] : std::vector<std::array<int, 3>>{{0, 2, 1}, {0, 1, 3}, {0, 3, 2}, {1, 2, 3}}) {
                    renderer.getWorld().getObjects().emplace_back(std::make_unique<eng::Triangle>(
                        points[i], points[j], points[k], eng::ColorType{i / 3.0f, j / 3.0f, k / 3.0f}));
                }

                for (int i = 0; i < 4; ++i)
                    for (int j = i + 1; j < 4; ++j)
                        renderer.getWorld().getObjects().emplace_back(
                            std::make_unique<eng::Line>(points[i], points[j], eng::ColorType(0.85f)));

                for (int i = 0; i < 4; ++i)
                    renderer.getWorld().getObjects().emplace_back(
                        std::make_unique<eng::Point>(points[i], eng::ColorType{0.8f, 0.2f, 0.3f}, 2));
            }

        renderer.clearScreen();
        renderer.renderSceneToScreen();

        window.clear();
        renderer.renderScreenToSFMLWindow(window);
        window.draw(text);
        window.display();
    }

    return 0;
}
