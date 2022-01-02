#include "SFMLRenderer.h"

SFMLRenderer::SFMLRenderer(Pixels width, Pixels height, sf::RenderWindow& mainAppWindow)
    : windowWidth_(width), windowHeight_(height), mainAppWindow_(mainAppWindow), renderer_(width, height) {
    screenTexture_.create(width, height);
    screenSprite_ = sf::Sprite(screenTexture_);

    // flip texture vertically
    screenSprite_.scale(1.0f, -1.0f);
    screenSprite_.setPosition(0, height);
}

size_t SFMLRenderer::render(eng::Scene& scene) {
    renderer_.clearScreen();
    size_t trianglesDrawn = renderer_.renderSceneToScreen(scene);

    eng::Screen& screen = renderer_.getScreen();

    Pixels width = screen.getWidth();
    Pixels height = screen.getHeight();

    screenTexture_.update(reinterpret_cast<const eng::Byte*>(screen.getColorBuffer().get_pointer()));
    mainAppWindow_.get().draw(screenSprite_);

    return trianglesDrawn;
}
