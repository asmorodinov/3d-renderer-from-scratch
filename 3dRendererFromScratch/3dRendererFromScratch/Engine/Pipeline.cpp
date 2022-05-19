#include "Pipeline.h"

#include <algorithm>
#include <array>

namespace eng {

// helper function

std::string getNextPipeline(std::string pipeline, bool next) {
    auto pipelines = std::vector<std::string>{"default", "hdr", "bloom", "blending", "blending with sort"};
    auto it = std::find(pipelines.begin(), pipelines.end(), pipeline);
    if (it == pipelines.end()) {
        assert(false);
        std::exit(1);
    }
    auto index = it - pipelines.begin();
    return pipelines[(index + (2 * static_cast<int>(next) - 1) + pipelines.size()) % pipelines.size()];
}

// negative

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

// bloom

BloomPipeline::BloomPipeline(Pixels width, Pixels height)
    : buffer_(width, height, Color128(0, 0, 0, 1)), bloom_buffer1_(width, height), bloom_buffer2_(width, height), result_(width, height) {
}

void blurIteration(ColorBuffer96& from, ColorBuffer96& to, int width, int height, bool horizontal) {
    static constexpr auto weights = std::array<float, 5>{0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f};

    // let compiler optimize this for least cache misses and best performance
    // microoptimizations are probably unnecessary here
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            auto res = weights[0] * from.get(x, y);

            for (int i = 1; i < 5; ++i) {
                if (horizontal) {
                    int x1 = std::max(0, x - i);
                    int x2 = std::min(width - 1, x + i);
                    res += weights[i] * (from.get(x1, y) + from.get(x2, y));
                } else {
                    int y1 = std::max(0, y - i);
                    int y2 = std::min(height - 1, y + i);
                    res += weights[i] * (from.get(x, y1) + from.get(x, y2));
                }
            }
            to.set(x, y, res);
        }
    }
}

void blur(size_t iterations, ColorBuffer96& buf1, ColorBuffer96& buf2, int width, int height) {
    for (size_t i = 0; i < iterations; ++i) {
        blurIteration(buf1, buf2, width, height, true);   // horizontal blur
        blurIteration(buf2, buf1, width, height, false);  // vertical blur
    }
}

PipelineResult BloomPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    // render pass in high dynamic range (Color128, no clamping)
    buffer_.clear();
    auto triangles = renderSceneToBuffer(scene, projectionInfo, buffer_);

    bloom_buffer1_.fill(Color128(0, 0, 0, 1));  // clear bloom buffer

    // extract only bright colors
    const auto& source = buffer_.getColorBuffer();
    for (size_t i = 0; i < buffer_.getWidth() * buffer_.getHeight(); ++i) {
        auto color = source.get(i);
        auto rgb = glm::vec3(color);
        auto brightness = glm::dot(rgb, glm::vec3(0.2126f, 0.7152f, 0.0722f));

        if (brightness > 1.0f) {
            bloom_buffer1_.get(i) = rgb;
        }
    }

    // blur bloom buffer
    blur(5, bloom_buffer1_, bloom_buffer2_, static_cast<int>(buffer_.getWidth()), static_cast<int>(buffer_.getHeight()));

    // add blurred buffer to the original
    for (size_t i = 0; i < buffer_.getWidth() * buffer_.getHeight(); ++i) {
        auto color = source.get(i);
        auto rgb = glm::vec3(color);
        rgb += bloom_buffer1_.get(i);  // add bloom

        result_.get(i) = HDRConversion<Color128, Color32>::convertColor(Color128(rgb, color.a));  // apply tone mapping and gamma correction and return result
    }

    return {triangles, result_.get_pointer()};
}

}  // namespace eng
