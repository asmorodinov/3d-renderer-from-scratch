#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Scene.h"
#include "ProjectionInfo.h"
#include "Types.h"
#include "Buffer.h"
#include "MeshTypes.h"

namespace eng {

template <typename Buffer>
size_t renderMesh(MeshVariant& mesh, const Camera& camera, ProjectionInfo& projectionInfo, const LightsVec& lights, Buffer& buffer) {
    return std::visit(
        [&](auto&& arg) -> size_t {
            arg.draw(camera, projectionInfo, lights, buffer);
            return arg.getTriangleCount();
        },
        mesh);
}

template <typename Buffer>
size_t renderSceneToBuffer(Scene& scene, ProjectionInfo& projectionInfo, Buffer& buffer) {
    const Camera& camera = scene.getCamera();
    const LightsVec& lights = scene.getPointLights();

    size_t trianglesDrawn = 0;

    for (auto& [name, mesh] : scene.getAllObjects()) {
        trianglesDrawn += renderMesh(mesh, camera, projectionInfo, lights, buffer);
    }

    static Mesh lightMesh = FlatMesh(Assets::getMeshData("light_bulb", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false);

    for (const auto& light : lights) {
        lightMesh.getTransform().setPosition(light.position);
        lightMesh.getTransform().setScale(glm::clamp(light.intensity, 0.1f, 5.0f) * glm::vec3(1.0f));
        lightMesh.setWireframeColor(convertColor<Color128, Color32>(Color128(light.color, 1.0f)));
        lightMesh.getFragmentShaderUniform().flatColor = light.color;

        lightMesh.draw(camera, projectionInfo, lights, buffer);

        trianglesDrawn += lightMesh.getTriangleCount();
    }

    return trianglesDrawn;
}

// just render scene to buffer (one render pass)
class DefaultPipeline {
 public:
    DefaultPipeline(Pixels width, Pixels height);
    size_t renderScene(Scene& scene, ProjectionInfo& projectionInfo);
    const ColorBuffer32& getResultBuffer() const;

 private:
    ColorAndDepthBuffer<Color32> buffer_;
};

// first render into buffer of Color128, then convert ColorBuffer128 to ColorBuffer32 (one render pass + buffer convertion)
class ConvertingPipeline {
 public:
    ConvertingPipeline(Pixels width, Pixels height);
    size_t renderScene(Scene& scene, ProjectionInfo& projectionInfo);
    const ColorBuffer32& getResultBuffer() const;

 private:
    ColorAndDepthBuffer<Color128> buffer_;
    ColorBuffer32 result_;
};

}  // namespace eng
