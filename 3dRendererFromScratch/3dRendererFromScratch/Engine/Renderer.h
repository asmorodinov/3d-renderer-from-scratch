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
#include "Pipeline.h"

namespace eng {

class Renderer {
 public:
    using Pipeline = DefaultPipeline;

 public:
    Renderer(Pixels width, Pixels height);

    ProjectionInfo& getProjectionInfo();
    Pipeline& getPipeline();

 private:
    ProjectionInfo projectionInfo_;
    Pipeline pipeline_;
};

}  // namespace eng
