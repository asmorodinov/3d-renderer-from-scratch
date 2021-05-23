#include "SampleScenes.h"

namespace eng {

Scene eng::makeScene1() {
    auto scene = Scene();

    static eng::MeshData skyboxMesh = eng::MeshData::generateCubeData(1.0f, true);

    float s = 2.6f;
    float h = -0.8;
    static eng::MeshData planeMesh = eng::MeshData{{{-s, h, s}, {-s, h, -s}, {s, h, s}, {s, h, -s}},
                                                   {{0.0f, 1.0f, 0.0f}},
                                                   {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                                                   {{0, 2, 1, 0, 1, 3, 0, 0, 0}, {3, 1, 2, 2, 3, 1, 0, 0, 0}}};

    float sz = 0.6f;
    static eng::MeshData cubeMesh = eng::MeshData::generateCubeData(sz);

    auto& lights = scene.getPointLights();

    lights.push_back(eng::PointLight({2, 0.1, 0}, {0.2, 1, 0.2}, 1.2f, 0.5f));
    lights.push_back(eng::PointLight({-2, 0.1, 0}, {1, 0.2, 0.2}, 1.2f, 0.5f));
    lights.push_back(eng::PointLight({0, 0.1, 2}, {0.2, 0.2, 1.0}, 1.2f, 0.5f));
    lights.push_back(eng::PointLight({0, 0.1, -2}, {0.2, 1.0, 1.0}, 1.2f, 0.5f));

    // objects.cubemapMeshes.push_back(
    //    eng::CubemapMesh(std::cref(skyboxMesh), {}, eng::Assets::getCubemapTexture("LancellottiChapel"), {}, false, false));

    // plane
    scene.addObject(eng::TextureMesh(std::cref(planeMesh), {}, eng::Assets::getTexture("textureStone.png"), {}));

    // cube

    scene.addObject(eng::TextureMesh(std::cref(cubeMesh), {}, eng::Assets::getTexture("crate.jpg"), {glm::vec3(-1.0f, h + sz, 0.8f)}));

    // cube 2
    scene.addObject(eng::TextureMesh(std::cref(cubeMesh), {}, eng::Assets::getTexture("brickwall.jpg"), {glm::vec3(-1.0f - 2.5f * sz, h + sz, 0.8f)}));

    // teapot
    scene.addObject(eng::FlatMesh(eng::Assets::getMeshData("teapot", 0.4f, false, true), {}, glm::vec3(1.0f, 1.0f, 1.0f), {glm::vec3(0.0f, h, 0.0f)}));

    /*
    // sphere
    for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 3; ++x) {
            objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/lowPolySphere.obj", 0.4f, true), texture2,
    eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader, normalMapShader));
            objects.back()->getTransform().position = glm::vec3(1.0f + 0.6f * x, h + 0.4f + 0.6f * y, -0.8f);
        }

    // sword
    objects.emplace_back(std::make_unique<eng::Mesh>(eng::loadFromObj("data/sword.obj", 2.0f, true), swordTexture,
    eng::ColorType(1.0f), phongShader, flatShader, textureShader, uvShader, normalShader, normalMapShader));
    objects.back()->getTransform().position = glm::vec3(-1.0f, h, -0.8f);

    eng::Mesh light(eng::loadFromObj("data/light.obj", 0.1f, true, true), texture, eng::ColorType(1.0f), phongShader, flatShader,
    textureShader, uvShader, normalShader, normalMapShader, eng::RenderMode::FlatColor);

    for (int y = 0; y < lights.size(); ++y) {
        glm::vec3 pos = lights[y].pos;
        glm::vec3 color = lights[y].color;

        auto lightObj = std::make_unique<eng::Mesh>(light);
        lightObj->setPosition(pos);
        lightObj->setColor(color);

        objects.emplace_back(std::move(lightObj));
    }
    */

    return scene;
}

}  // namespace eng
