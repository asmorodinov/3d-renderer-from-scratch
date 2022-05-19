#pragma once

#include <cstdint>
#include <string>
#include <algorithm>
#include <vector>
#include <string_view>
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
#include "Blending.h"
#include "Discard.h"

namespace eng {

template <typename Buffer>
size_t renderLightsToBuffer(Scene& scene, ProjectionInfo& projectionInfo, Buffer& buffer) {
    const Camera& camera = scene.getCamera();
    const LightsVec& lights = scene.getPointLights();

    size_t trianglesDrawn = 0;

    static Mesh lightMesh = FlatMesh(Assets::getMeshData("light_bulb", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false);

    for (const auto& light : lights) {
        lightMesh.getTransform().setPosition(light.position);
        lightMesh.getTransform().setScale(glm::clamp(light.intensity, 0.1f, 5.0f) * glm::vec3(1.0f));
        lightMesh.setWireframeColor(DefaultConversion<Color128, Color32>::convertColor(Color128(light.color, 1.0f)));
        lightMesh.getFragmentShaderUniform().flatColor = light.color * light.intensity;

        lightMesh.draw(camera, projectionInfo, lights, buffer);

        trianglesDrawn += lightMesh.getTriangleCount();
    }

    return trianglesDrawn;
}

// normal scene rendering

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

    size_t trianglesDrawn = renderLightsToBuffer(scene, projectionInfo, buffer);

    for (auto& [name, mesh] : scene.getAllObjects()) {
        trianglesDrawn += renderMesh(mesh, camera, projectionInfo, lights, buffer);
    }

    return trianglesDrawn;
}

// scene rendering with transparent objects sorting based on distance

inline std::vector<Distance> getMeshDistances(MeshVariant& mesh, const Camera& camera) {
    return std::visit([&](auto&& arg) -> std::vector<Distance> { return arg.getDistances(camera); }, mesh);
}
inline bool isTransparent(const MeshVariant& mesh) {
    return std::visit([&](auto&& arg) -> bool { return arg.getIsTransparent(); }, mesh);
}
template <typename Buffer>
void drawMeshFace(MeshVariant& mesh, const Camera& camera, ProjectionInfo& projectionInfo, const LightsVec& lights, Buffer& buffer, size_t faceIndex) {
    std::visit([&](auto&& arg) -> void { arg.drawFace(camera, projectionInfo, lights, buffer, faceIndex); }, mesh);
}

struct TransparentTriangle {
    std::string_view meshName;
    size_t faceIndex;
    float distance;
};

template <typename Buffer>
size_t renderSceneToBufferWithTransparentObjects(Scene& scene, ProjectionInfo& projectionInfo, Buffer& buffer) {
    const Camera& camera = scene.getCamera();
    const LightsVec& lights = scene.getPointLights();

    size_t trianglesDrawn = renderLightsToBuffer(scene, projectionInfo, buffer);

    std::vector<TransparentTriangle> triangles;

    auto& objects = scene.getAllObjects();
    for (auto& [name, mesh] : objects) {
        if (!isTransparent(mesh)) {
            trianglesDrawn += renderMesh(mesh, camera, projectionInfo, lights, buffer);
            continue;
        }
        auto distances = getMeshDistances(mesh, camera);
        for (size_t i = 0; i < distances.size(); ++i) {
            triangles.push_back(TransparentTriangle{name, i, distances[i]});
        }
        trianglesDrawn += distances.size();
    }

    std::sort(triangles.begin(), triangles.end(), [](const TransparentTriangle& a, const TransparentTriangle& b) { return a.distance > b.distance; });

    for (const auto& triangle : triangles) {
        auto& mesh = objects.at(std::string(triangle.meshName));
        drawMeshFace(mesh, camera, projectionInfo, lights, buffer, triangle.faceIndex);
    }

    return trianglesDrawn;
}

// pipelines

struct PipelineResult {
    size_t trianglesCount;
    const Color32* buffer;
};

std::string getNextPipeline(std::string pipeline, bool next);

// just render scene to buffer (one render pass)
template <template <typename Color1, typename Color2> class Conversion, template <typename Color> class Blending, template <typename Color> class Discard>
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
    ColorAndDepthBuffer<Color32, Conversion, Blending, Discard> buffer_;
};

using DefaultPipeline = OnePassPipeline<ClampConversion, NoBlending, NoDiscard>;
using HDRPipeline = OnePassPipeline<HDRConversion, NoBlending, NoDiscard>;
using BlendingPipeline = OnePassPipeline<ClampConversion, AlphaBlending, AlphaDiscard>;

// first render into buffer of Color128, then convert ColorBuffer128 to ColorBuffer32 (one render pass + buffer convertion)
class ConvertingPipeline {
 public:
    ConvertingPipeline(Pixels width, Pixels height);
    PipelineResult renderScene(Scene& scene, ProjectionInfo& projectionInfo);

 private:
    ColorAndDepthBuffer<Color128, DefaultConversion, NoBlending, NoDiscard> buffer_;
    ColorBuffer32 result_;
};

// bloom

class BloomPipeline {
 public:
    BloomPipeline(Pixels width, Pixels height);
    PipelineResult renderScene(Scene& scene, ProjectionInfo& projectionInfo);

 private:
    ColorAndDepthBuffer<Color128, DefaultConversion, NoBlending, AlphaDiscard> buffer_;
    ColorBuffer96 bloom_buffer1_;  // we are not using alpha channel for bloom, only rgb
    ColorBuffer96 bloom_buffer2_;
    ColorBuffer32 result_;
};

// transparent pipelines

template <template <typename Color1, typename Color2> class Conversion, template <typename Color> class Blending, template <typename Color> class Discard>
class OnePassTransparentPipeline {
 public:
    OnePassTransparentPipeline(Pixels width, Pixels height) : buffer_(width, height, Color32{0, 0, 0, 255}) {
    }
    PipelineResult renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
        buffer_.clear();
        auto tr = renderSceneToBufferWithTransparentObjects(scene, projectionInfo, buffer_);
        return {tr, buffer_.getColorBuffer().get_pointer()};
    }

 private:
    ColorAndDepthBuffer<Color32, Conversion, Blending, Discard> buffer_;
};

using BlendingSortingPipeline = OnePassTransparentPipeline<ClampConversion, AlphaBlending, AlphaDiscard>;

}  // namespace eng
