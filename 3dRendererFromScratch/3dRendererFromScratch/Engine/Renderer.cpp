#include "Renderer.h"

namespace eng {

Renderer::Renderer(size_t width, size_t height) : screen_(width, height, 0.2f * glm::vec3(0.2f, 0.25f, 0.25f)) {}

Screen& Renderer::getScreen() { return screen_; }

void Renderer::clearScreen() { screen_.clear(); }

template <std::size_t I = 0, typename... Tp>
typename std::enable_if<I == sizeof...(Tp), size_t>::type draw(std::tuple<Tp...>& t, const Camera& camera, Screen& screen, const LightsVec& lights) {
    return 0;
}
template <std::size_t I = 0, typename... Tp>
    typename std::enable_if < I<sizeof...(Tp), size_t>::type draw(std::tuple<Tp...>& t, const Camera& camera, Screen& screen, const LightsVec& lights) {
    size_t trianglesDrawn = 0;
    for (auto& object : std::get<I>(t)) {
        object.draw(camera, screen, lights);
        trianglesDrawn += object.getTriangleCount();
    }
    trianglesDrawn += draw<I + 1, Tp...>(t, camera, screen, lights);

    return trianglesDrawn;
}

size_t Renderer::renderSceneToScreen(Scene& scene) {
    const Camera& camera = scene.getCamera();
    const LightsVec& lights = scene.getPointLights();

    return draw(scene.getAllObjects(), camera, screen_, lights);
}

void Renderer::renderScreenToFile(const std::string& file) const {
    size_t width = screen_.getWidth();
    size_t height = screen_.getHeight();

    std::vector<uint8_t> pixels(width * height * 3, 0);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            Color color = screen_.getPixelColor(x, y);

            pixels[3 * (y * width + x) + 0] = uint8_t(255.99f * color.r);
            pixels[3 * (y * width + x) + 1] = uint8_t(255.99f * color.g);
            pixels[3 * (y * width + x) + 2] = uint8_t(255.99f * color.b);
        }
    }

    assert(file.size() >= 4);
    auto extension = file.substr(file.size() - 4);

    if (extension == ".png") {
        stbi_write_png(file.c_str(), width, height, 3, &pixels[0], width * 3);
    } else if (extension == ".jpg") {
        stbi_write_jpg(file.c_str(), width, height, 3, &pixels[0], 100);
    } else {
        assert(0);
    }
}

}  // namespace eng
