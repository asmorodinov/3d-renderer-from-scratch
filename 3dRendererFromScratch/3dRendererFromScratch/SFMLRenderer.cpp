#include "SFMLRenderer.h"

SFMLRenderer::SFMLRenderer(Pixels width, Pixels height, sf::RenderWindow& mainAppWindow)
    : windowWidth_(width), windowHeight_(height), mainAppWindow_(mainAppWindow), renderer_(width, height) {
    screenTexture_.create(width, height);
    screenSprite_ = sf::Sprite(screenTexture_);
}

size_t SFMLRenderer::render(eng::Scene& scene) {
    renderer_.clearScreen();
    size_t trianglesDrawn = renderer_.renderSceneToScreen(scene);

    eng::Screen& screen = renderer_.getScreen();

    Pixels width = screen.getWidth();
    Pixels height = screen.getHeight();

    static std::vector<sf::Uint8> pixels(width * height * 4, 0);

    for (Pixels x = 0; x < width; ++x) {
        for (Pixels y = 0; y < height; ++y) {
            Color color = screen.getPixelColor(x, height - 1 - y);
            color = glm::min(color, 1.0f);

            pixels[4 * (y * width + x) + 0] = sf::Uint8(255.99f * color.r);
            pixels[4 * (y * width + x) + 1] = sf::Uint8(255.99f * color.g);
            pixels[4 * (y * width + x) + 2] = sf::Uint8(255.99f * color.b);
            pixels[4 * (y * width + x) + 3] = sf::Uint8(255);
        }
    }

    screenTexture_.update(&pixels[0]);
    mainAppWindow_.get().draw(screenSprite_);

    return trianglesDrawn;
}
