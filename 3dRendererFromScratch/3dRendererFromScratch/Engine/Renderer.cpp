#include "Renderer.h"

namespace eng {

Renderer::Renderer(Pixels width, Pixels height) : projectionInfo_(width, height), pipeline_(width, height) {
}

ProjectionInfo& Renderer::getProjectionInfo() {
    return projectionInfo_;
}

Renderer::Pipeline& Renderer::getPipeline() {
    return pipeline_;
}

}  // namespace eng
