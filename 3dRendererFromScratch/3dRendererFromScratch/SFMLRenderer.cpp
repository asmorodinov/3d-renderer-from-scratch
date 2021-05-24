#include "SFMLRenderer.h"

SFMLRenderer::SFMLRenderer(size_t width, size_t height, sf::RenderWindow& window) : width(width), height(height), window(window), renderer(width, height) {
    screenTexture.create(width, height);
    screenSprite = sf::Sprite(screenTexture);
}

size_t SFMLRenderer::render(eng::Scene& scene) {
    renderer.clearScreen();
    size_t trianglesDrawn = renderer.renderSceneToScreen(scene);

    eng::Screen& screen = renderer.getScreen();

    size_t width = screen.getWidth();
    size_t height = screen.getHeight();

    std::vector<sf::Uint8> pixels(width * height * 4, 0);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            eng::ColorType color = screen.getPixelColor(x, height - 1 - y);
            color = glm::min(color, 1.0f);

            pixels[4 * (y * width + x) + 0] = sf::Uint8(255.99f * color.r);
            pixels[4 * (y * width + x) + 1] = sf::Uint8(255.99f * color.g);
            pixels[4 * (y * width + x) + 2] = sf::Uint8(255.99f * color.b);
            pixels[4 * (y * width + x) + 3] = sf::Uint8(255);
        }
    }

    screenTexture.update(&pixels[0]);
    window.get().draw(screenSprite);

    return trianglesDrawn;
}
