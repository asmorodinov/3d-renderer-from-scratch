#pragma once

#include <cstdint>
#include <string>

#include <SFML/Graphics.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include "Scene.h"
#include "ProjectionInfo.h"
#include "Types.h"
#include "Buffer.h"
#include "Pipeline.h"

namespace eng {

class Renderer {
 public:
    Renderer(Pixels width, Pixels height);

    ProjectionInfo& getProjectionInfo();

    PipelineResult renderScene(Scene& scene);

 private:
    ProjectionInfo projectionInfo_;
    DefaultPipeline defaultPipeline_;
    HDRPipeline hdrPipeline_;
    BloomPipeline bloomPipeline_;
    ConvertingPipeline convertingPipeline_;
};

}  // namespace eng
