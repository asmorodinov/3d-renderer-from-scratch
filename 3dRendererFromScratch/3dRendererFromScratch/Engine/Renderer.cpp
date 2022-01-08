#include "Renderer.h"

namespace eng {

Renderer::Renderer(Pixels width, Pixels height) : screen_(width, height, glm::vec3(0.0f)) {
}

Screen& Renderer::getScreen() {
    return screen_;
}

void Renderer::clearScreen() {
    screen_.clear();
}

size_t drawMesh(MeshVariant& mesh, const Camera& camera, Screen& screen, const LightsVec& lights) {
    return std::visit(
        [&](auto&& arg) -> size_t {
            arg.draw(camera, screen, lights);
            return arg.getTriangleCount();
        },
        mesh);
}

size_t Renderer::renderSceneToScreen(Scene& scene) {
    const Camera& camera = scene.getCamera();
    const LightsVec& lights = scene.getPointLights();

    size_t trianglesDrawn = 0;

    for (auto& [name, mesh] : scene.getAllObjects()) {
        trianglesDrawn += drawMesh(mesh, camera, screen_, lights);
    }

    static Mesh lightMesh = FlatMesh(Assets::getMeshData("light_bulb", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false);

    for (const auto& light : lights) {
        lightMesh.getTransform().setPosition(light.position);
        lightMesh.getTransform().setScale(glm::clamp(light.intensity, 0.1f, 5.0f) * glm::vec3(1.0f));
        lightMesh.setWireframeColor(light.color);
        lightMesh.getFragmentShaderUniform().flatColor = light.color;

        lightMesh.draw(camera, screen_, lights);

        trianglesDrawn += lightMesh.getTriangleCount();
    }

    return trianglesDrawn;
}

void Renderer::renderScreenToFile(const std::string& file) const {
    Pixels width = screen_.getWidth();
    Pixels height = screen_.getHeight();

    std::vector<uint8_t> pixels(width * height * 3, 0);

    for (Pixels x = 0; x < width; ++x) {
        for (Pixels y = 0; y < height; ++y) {
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
