#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstdlib>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Scene.h"
#include "ProjectionInfo.h"
#include "Types.h"
#include "Buffer.h"
#include "MeshTypes.h"
#include "Conversion.h"

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
        lightMesh.setWireframeColor(DefaultConversion<Color128, Color32>::convertColor(Color128(light.color, 1.0f)));
        lightMesh.getFragmentShaderUniform().flatColor = light.color;

        lightMesh.draw(camera, projectionInfo, lights, buffer);

        trianglesDrawn += lightMesh.getTriangleCount();
    }

    return trianglesDrawn;
}

struct PipelineResult {
    size_t trianglesCount;
    const Color32* buffer;
};

inline std::string getNextPipeline(std::string pipeline) {
    auto pipelines = std::vector<std::string>{"default", "hdr"};
    auto it = std::find(pipelines.begin(), pipelines.end(), pipeline);
    if (it == pipelines.end()) {
        assert(false);
        std::exit(1);
    }
    auto index = it - pipelines.begin();
    return pipelines[(index + 1) % pipelines.size()];
}

// just render scene to buffer (one render pass)
template <template <typename Color1, typename Color2> class Conversion>
class OnePassPipeline {
 public:
    OnePassPipeline(Pixels width, Pixels height) : buffer_(width, height, Color32{0, 0, 0, 255}) {
    }
    PipelineResult renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
        buffer_.clear();
        auto tr = renderSceneToBuffer(scene, projectionInfo, buffer_);
        return {tr, buffer_.getColorBuffer().get_pointer()};
    }

 private:
    ColorAndDepthBuffer<Color32, Conversion> buffer_;
};

using DefaultPipeline = OnePassPipeline<ClampConversion>;
using HDRPipeline = OnePassPipeline<HDRConversion>;

// first render into buffer of Color128, then convert ColorBuffer128 to ColorBuffer32 (one render pass + buffer convertion)
class ConvertingPipeline {
 public:
    ConvertingPipeline(Pixels width, Pixels height);
    PipelineResult renderScene(Scene& scene, ProjectionInfo& projectionInfo);

 private:
    ColorAndDepthBuffer<Color128, DefaultConversion> buffer_;
    ColorBuffer32 result_;
};

}  // namespace eng
