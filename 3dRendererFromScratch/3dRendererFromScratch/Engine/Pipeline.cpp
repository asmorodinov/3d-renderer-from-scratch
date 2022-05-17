#include "Pipeline.h"

namespace eng {

ConvertingPipeline::ConvertingPipeline(Pixels width, Pixels height) : buffer_(width, height, Color128{0.0f, 0.0f, 0.0f, 1.0f}), result_(width, height) {
}

PipelineResult ConvertingPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    buffer_.clear();
    auto res = renderSceneToBuffer(scene, projectionInfo, buffer_);

    const auto& source = buffer_.getColorBuffer();
    for (size_t i = 0; i < buffer_.getWidth() * buffer_.getHeight(); ++i) {
        auto color = 1.0f - source.get(i);
        color.a = 1.0f;
        result_.get(i) = ClampConversion<Color128, Color32>::convertColor(color);
    }

    return {res, result_.get_pointer()};
}

}  // namespace eng
