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
    auto& info = renderer_.getProjectionInfo();
    auto& pipeline = renderer_.getPipeline();

    size_t trianglesDrawn = pipeline.renderScene(scene, info);

    Pixels width = info.getWidth();
    Pixels height = info.getHeight();

    screenTexture_.update(reinterpret_cast<const eng::Byte*>(pipeline.getResultBuffer().get_pointer()));
    mainAppWindow_.get().draw(screenSprite_);

    return trianglesDrawn;
}
