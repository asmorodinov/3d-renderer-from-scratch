#include "Renderer.h"

#include <cassert>
#include <cstdlib>

namespace eng {

Renderer::Renderer(Pixels width, Pixels height) : projectionInfo_(width, height), defaultPipeline_(width, height), convertingPipeline_(width, height) {
}

ProjectionInfo& Renderer::getProjectionInfo() {
    return projectionInfo_;
}

PipelineResult Renderer::renderScene(Scene& scene) {
    auto pipeline = scene.getPipeline();
    if (pipeline == "default") {
        return defaultPipeline_.renderScene(scene, projectionInfo_);
    } else if (pipeline == "converting") {
        return convertingPipeline_.renderScene(scene, projectionInfo_);
    }
    assert(false);
    exit(1);
}

}  // namespace eng
