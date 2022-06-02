#include "Pipeline.h"

#include <algorithm>
#include <array>
#include <utility>
#include <random>

#include "Assets.h"

namespace eng {

// helper function

std::string getNextPipeline(std::string pipeline, bool next) {
    auto pipelines = std::vector<std::string>{"default", "hdr", "bloom", "shadow mapping", "blending", "blending with sort", "deferred shading", "SSAO"};
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

// shadow mapping

ShadowMappingPipeline::ShadowMappingPipeline(Pixels width, Pixels height, Pixels depthMapWidth, Pixels depthMapHeight)
    : depthMapBuffer_(depthMapWidth, depthMapHeight, Color32(0, 0, 0, 255)),
      result_(width, height, Color32(0, 0, 0, 255)),
      mesh_(Assets::getMeshData("cube", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false) {
}

PipelineResult ShadowMappingPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    // 1) render scene and get depth map
    const auto& camera = scene.getCamera();
    const auto& lights = scene.getPointLights();

    // no shadows
    if (lights.size() == 0) {
        result_.clear();
        auto tr = renderSceneToBuffer(scene, projectionInfo, result_);
        return {tr, result_.getColorBuffer().get_pointer()};
    }

    // get light information
    auto light = lights.front();
    auto lightPos = light.position;
    auto lightDir = glm::normalize(-lightPos);  // light is always directed towards center of the scene, this is just a temporary placeholder until directioanl
                                                // lights are properly implemented.
    if (lightDir == glm::vec3(0, -1, 0)) {
        // can not look directly below when using glm::lookAt
        lightDir = glm::normalize(lightDir + glm::vec3(0.001f, 0.0f, 0.0f));
    }

    auto lightCamera = Camera(lightPos, lightDir);
    auto lightProjectionInfo = ProjectionInfo(depthMapBuffer_.getWidth(), depthMapBuffer_.getHeight());  // perspective projection

    // render into depthMapBuffer from light perspective to get depth map
    depthMapBuffer_.clear();
    for (auto& [name, mesh] : scene.getAllObjects()) {
        std::visit([&](auto&& arg) -> void { copyMeshParamsToOtherMesh(mesh_, arg); }, mesh);  // copy parameters to flat mesh
        auto meshVar = MeshVariant{mesh_};
        renderMesh(meshVar, lightCamera, lightProjectionInfo, lights, depthMapBuffer_);  // render flat mesh
    }

    // now we have depth map
    const auto& depthMap = depthMapBuffer_.getDepthBuffer();
    auto lightSpaceMatrix = lightProjectionInfo.getProjectionMatrix() * lightCamera.getViewMatrix();
    Assets::setShadowMappingInfo(ShadowMappingInfo{depthMap, lightSpaceMatrix});

    // 2) render scene with lighting and depth map information
    result_.clear();
    auto tr = renderSceneToBuffer(scene, projectionInfo, result_);

    // remove shadow mapping info
    Assets::setShadowMappingInfo(std::nullopt);

    return {tr, result_.getColorBuffer().get_pointer()};
}

// deferred shading

DeferredShadingPipeline::DeferredShadingPipeline(Pixels width, Pixels height)
    : gBuffer_(width, height, GeometryInfo{}),
      result_(width, height, Color32(0, 0, 0, 255)),
      mesh_(Assets::getMeshData("cube", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false) {
}

PipelineResult DeferredShadingPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    // geometry pass
    const auto& camera = scene.getCamera();
    const auto& lights = scene.getPointLights();

    // render scene geometry into gBuffer
    gBuffer_.clear();
    size_t tr = 0;
    for (auto& [name, mesh] : scene.getAllObjects()) {
        std::visit([&](auto&& arg) -> void { copyMeshParamsToOtherMesh(mesh_, arg); }, mesh);  // copy parameters to flat mesh

        mesh_.draw(camera, projectionInfo, lights, gBuffer_);  // render mesh
        tr += mesh_.getTriangleCount();
    }

    // lighting pass
    result_.clear();
    const auto& source = gBuffer_.getColorBuffer();
    for (size_t i = 0; i < gBuffer_.getWidth() * gBuffer_.getHeight(); ++i) {
        auto info = source.get(i);
        auto lighting = DeferredPhongShader::computePixelColor(camera.getPosition(), info, lights);
        result_.getColorBuffer().get(i) = ClampConversion<Color128, Color32>::convertColor(lighting);
    }

    // also render lights (forward rendering), but before that copy depth buffer from deferred pass to forward pass
    result_.getDepthBuffer() = gBuffer_.getDepthBuffer();

    tr += renderLightsToBuffer(scene, projectionInfo, result_);

    return {tr, result_.getColorBuffer().get_pointer()};
}

// SSAO

SSAOPipeline::SSAOPipeline(Pixels width, Pixels height)
    : gBuffer_(width, height, GeometryInfo{}),
      result_(width, height, Color32(0, 0, 0, 255)),
      mesh_(Assets::getMeshData("cube", 0.05f), {}, FragmentShaderUniform(), ObjectTransform(), false),
      occlusionTexture(width, height, 0.0f),
      blurredOcclusionTexture(width, height, 0.0f) {
    // generate sample kernel
    // ----------------------
    auto lerp = [](float a, float b, float f) { return a + f * (b - a); };

    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);  // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;

    for (size_t i = 0; i < sampleCount; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / sampleCount;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    for (size_t i = 0; i < noiseTextureSize * noiseTextureSize; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f);  // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
}

PipelineResult SSAOPipeline::renderScene(Scene& scene, ProjectionInfo& projectionInfo) {
    // geometry pass

    const auto& camera = scene.getCamera();
    const auto& lights = scene.getPointLights();
    const auto& projection = projectionInfo.getProjectionMatrix();
    auto width = gBuffer_.getWidth();
    auto height = gBuffer_.getHeight();

    auto cameraSpaceLights = lights;
    for (auto& light : cameraSpaceLights) {
        light.position = glm::vec3(camera.getViewMatrix() * glm::vec4(light.position, 1.0f));
    }

    // render scene geometry into gBuffer
    gBuffer_.clear();
    size_t tr = 0;
    for (auto& [name, mesh] : scene.getAllObjects()) {
        std::visit([&](auto&& arg) -> void { copyMeshParamsToOtherMesh(mesh_, arg); }, mesh);  // copy parameters to flat mesh

        mesh_.draw(camera, projectionInfo, lights, gBuffer_);  // render mesh
        tr += mesh_.getTriangleCount();
    }

    // SSAO
    static constexpr float radius = 0.5f;
    static constexpr float bias = 0.025f;
    for (size_t x = 0; x < width; ++x) {
        for (size_t y = 0; y < height; ++y) {
            const auto& info = gBuffer_.getPixelColor(x, y);

            auto fragPos = info.position;
            auto normal = info.normal;
            auto randomVec = ssaoNoise[(y % noiseTextureSize) * noiseTextureSize + (x % noiseTextureSize)];

            auto tangent = glm::normalize(randomVec - normal * glm::dot(randomVec, normal));
            auto bitangent = glm::cross(normal, tangent);
            auto TBN = glm::mat3(tangent, bitangent, normal);

            auto occlusion = 0.0f;
            for (size_t i = 0; i < sampleCount; ++i) {
                auto samplePos = fragPos + radius * (TBN * ssaoKernel[i]);

                auto offset = projection * glm::vec4(samplePos, 1.0f);
                offset /= offset.w;
                offset = offset * 0.5f + 0.5f;

                auto sampleDepth = sample2dBuffer(gBuffer_.getColorBuffer(), glm::vec2(offset.x, 1.0f - offset.y)).position.z;

                auto rangeCheck = glm::smoothstep(0.0f, 1.0f, radius / glm::abs(fragPos.z - sampleDepth));
                occlusion += (sampleDepth >= samplePos.z + bias ? 1.0f : 0.0f) * rangeCheck;
            }
            occlusion = 1.0f - (occlusion / sampleCount);

            occlusionTexture.set(x, y, occlusion);
        }
    }

    // blur occlusion texture
    auto widthInt = static_cast<int>(width);
    auto heightInt = static_cast<int>(height);
    auto range = static_cast<int>(noiseTextureSize / 2);
    for (int x = 0; x < widthInt; ++x) {
        for (int y = 0; y < heightInt; ++y) {
            auto result = 0.0f;
            for (int dx = -range; dx < range; ++dx) {
                for (int dy = -range; dy < range; ++dy) {
                    auto x1 = std::max(0, std::min(x + dx, widthInt - 1));
                    auto y1 = std::max(0, std::min(y + dy, heightInt - 1));
                    result += occlusionTexture.get(x1, y1);
                }
            }
            blurredOcclusionTexture.set(size_t(x), size_t(y), result / (noiseTextureSize * noiseTextureSize));
        }
    }

    // lighting pass with occlusion info (everything is in camera space)

    result_.clear();
    const auto& source = gBuffer_.getColorBuffer();
    for (size_t i = 0; i < gBuffer_.getWidth() * gBuffer_.getHeight(); ++i) {
        auto occlusion = blurredOcclusionTexture.get(i);
        
        auto info = source.get(i);
        auto lighting = SSAOPhongShader::computePixelColor(glm::vec3(0.0f), occlusion, info, cameraSpaceLights);
        
        result_.getColorBuffer().get(i) = ClampConversion<Color128, Color32>::convertColor(lighting);
    }

    // also render lights (forward rendering), but before that copy depth buffer from deferred pass to forward pass
    result_.getDepthBuffer() = gBuffer_.getDepthBuffer();

    tr += renderLightsToBuffer(scene, projectionInfo, result_);

    return {tr, result_.getColorBuffer().get_pointer()};
}

}  // namespace eng
