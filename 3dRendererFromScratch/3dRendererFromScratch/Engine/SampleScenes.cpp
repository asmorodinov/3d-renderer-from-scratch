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

    // skybox
    scene.addObject(eng::CubemapMesh(std::cref(skyboxMesh), {}, eng::Assets::getCubemapTexture("LancellottiChapel"), {}, false, false));

    // plane
    scene.addObject(eng::TextureMesh(std::cref(planeMesh), {}, eng::Assets::getTexture("textureStone.png"), {}));

    // cubicAttenuationCoefficient
    scene.addObject(eng::TextureMesh(std::cref(cubeMesh), {}, eng::Assets::getTexture("crate.jpg"), {glm::vec3(-1.0f, h + sz, 0.8f)}));

    // cubicAttenuationCoefficient 2
    scene.addObject(eng::NormalMapMesh(std::cref(cubeMesh), {}, {eng::Assets::getTexture("brickwall.jpg"), eng::Assets::getTexture("brickwall_normal.jpg")},
                                       {glm::vec3(-1.0f - 2.5f * sz, h + sz, 0.8f)}));

    // teapot
    scene.addObject(eng::PhongMesh(eng::Assets::getMeshData("teapot", 0.4f, false, true), {},
                                   {eng::Assets::getTexture("texture3.png"), eng::Assets::getCubemapTexture("LancellottiChapel")}, {glm::vec3(0.0f, h, 0.0f)}));

    // sphere
    for (int y = 0; y < 2; ++y)
        for (int x = 0; x < 3; ++x) {
            scene.addObject(eng::NormalMesh(eng::Assets::getMeshData("lowPolySphere", 0.4f), {}, {}, {glm::vec3(1.0f + 0.6f * x, h + 0.4f + 0.6f * y, -0.8f)}));
        }

    // sword
    scene.addObject(eng::UVMesh(eng::Assets::getMeshData("sword", 2.0f), {}, {}, {glm::vec3(-1.0f, h, -0.8f)}));

    // lights
    eng::FlatMesh light(eng::Assets::getMeshData("light", 0.1f, false, true), {}, {}, {});

    for (int y = 0; y < lights.size(); ++y) {
        glm::vec3 pos = lights[y].position;
        glm::vec3 color = lights[y].color;

        auto lightObj = light;
        lightObj.getTransform().setPosition(pos);
        lightObj.setDrawingMode(true, true, true, color);

        scene.addObject(std::move(lightObj));
    }

    return scene;
}

}  // namespace eng
