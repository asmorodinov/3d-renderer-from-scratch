#include "Pipeline.h"

namespace eng {

DefaultPipeline::DefaultPipeline(Pixels width, Pixels height) : buffer_(width, height, Color32{0, 0, 0, 255}) {
}

size_t DefaultPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    buffer_.clear();
    return renderSceneToBuffer(scene, projectionInfo, buffer_);
}

const ColorBuffer32& DefaultPipeline::getResultBuffer() const {
    return buffer_.getColorBuffer();
}

ConvertingPipeline::ConvertingPipeline(Pixels width, Pixels height) : buffer_(width, height, Color128{0.0f, 0.0f, 0.0f, 1.0f}), result_(width, height) {
}

size_t ConvertingPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    buffer_.clear();
    auto res = renderSceneToBuffer(scene, projectionInfo, buffer_);

    const auto& source = buffer_.getColorBuffer();
    for (size_t i = 0; i < buffer_.getWidth() * buffer_.getHeight(); ++i) {
        result_.get(i) = convertColor<Color128, Color32>(source.get(i));
    }

    return res;
}

const ColorBuffer32& ConvertingPipeline::getResultBuffer() const {
    return result_;
}

}  // namespace eng
