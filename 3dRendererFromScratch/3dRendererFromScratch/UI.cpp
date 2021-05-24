#include "UI.h"

#include <cstdlib>

UI::UI(sf::RenderWindow& window_) : window(window_) {
    if (!font.loadFromFile("data/fonts/arial.ttf")) {
        std::cout << "Failed to load font\n";
        exit(1);
    }

    window.get().setMouseCursorVisible(false);

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

void UI::updateAndDraw(float dt, size_t trianglesCount) {
    time += dt;
    ++frames;

    if (time - lastTime >= 1.5f) {
        fps = frames / (time - lastTime);
        text.setString("FPS: " + std::to_string(static_cast<int>(fps)));
        text2.setString("ms per frame: " + std::to_string(static_cast<int>(1000.0f / fps)));
        frames = 0;
        lastTime = time;
    }

    text3.setString("triangles: " + std::to_string(trianglesCount));

    window.get().draw(text);
    window.get().draw(text2);
    window.get().draw(text3);
}
