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
    auto res = renderer_.renderScene(scene);

    screenTexture_.update(reinterpret_cast<const eng::Byte*>(res.buffer));
    mainAppWindow_.get().draw(screenSprite_);

    return res.trianglesCount;
}
