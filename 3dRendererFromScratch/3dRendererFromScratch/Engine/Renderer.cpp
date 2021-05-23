#include "Renderer.h"

namespace eng {

Renderer::Renderer(size_t width, size_t height) : screen(width, height, 0.2f * glm::vec3(0.2f, 0.25f, 0.25f)) {
    screenTexture.create(width, height);
    screenSprite = sf::Sprite(screenTexture);
}

Screen& Renderer::getScreen() { return screen; }
Scene& Renderer::getScene() { return world; }

void Renderer::clearScreen() { screen.clear(); }

void Renderer::mouseMove(float x, float y) {
    if (mx >= 0.0f && my >= 0.0f && x >= 0.0f && y >= 0.0f) {
        camera.mouseMove(x - mx, y - my);
    }
    mx = x;
    my = y;
}

void Renderer::keyPressedOrReleased(sf::Keyboard::Key key, bool mode) { camera.keyPressedOrReleased(key, mode); }

void Renderer::update(float dt) {
    camera.update(dt);

    if (playerControl) {
        world.getCamera().setPosition(camera.position);
        world.getCamera().setDirection(camera.direction);
    }
}

template <typename T>
size_t draw(std::pmr::vector<T>& vec, const Camera& camera, Screen& screen, const LightsVec& lights) {
    size_t trianglesDrawn = 0;
    for (auto& object : vec) {
        object.draw(camera, screen, lights);
        trianglesDrawn += object.getTriangleCount();
    }
    return trianglesDrawn;
}

size_t Renderer::renderSceneToScreen() {
    size_t trianglesDrawn = 0;
    const Camera& camera = world.getCamera();
    const LightsVec& lights = world.getPointLights();
    ObjectsVec& objects = world.getObjects();
    trianglesDrawn += draw(objects.cubemapMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.flatMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.normalMapMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.normalMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.phongMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.textureMeshes, camera, screen, lights);
    trianglesDrawn += draw(objects.uvMeshes, camera, screen, lights);

    return trianglesDrawn;
}

void Renderer::renderScreenToFile(const std::string& file) const {
    size_t width = screen.getWidth();
    size_t height = screen.getHeight();

    std::vector<uint8_t> pixels(width * height * 3, 0);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            ColorType color = screen.getPixelColor(x, y);

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

void Renderer::renderScreenToSFMLWindow(sf::RenderWindow& window) {
    size_t width = screen.getWidth();
    size_t height = screen.getHeight();

    std::vector<sf::Uint8> pixels(width * height * 4, 0);

    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            ColorType color = screen.getPixelColor(x, height - 1 - y);
            color = glm::min(color, 1.0f);

            pixels[4 * (y * width + x) + 0] = sf::Uint8(255.99f * color.r);
            pixels[4 * (y * width + x) + 1] = sf::Uint8(255.99f * color.g);
            pixels[4 * (y * width + x) + 2] = sf::Uint8(255.99f * color.b);
            pixels[4 * (y * width + x) + 3] = sf::Uint8(255);
        }
    }

    screenTexture.update(&pixels[0]);
    window.draw(screenSprite);
}

}  // namespace eng
