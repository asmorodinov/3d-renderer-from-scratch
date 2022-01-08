#pragma once

#include "BasicShader.h"
#include "TextureShader.h"

namespace eng {

struct UVShader {
    using Uniform = FragmentShaderUniform;
    using VertexShaderOutput = EmptyStruct;
    using Var = OneVariable<glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

using UVVertexShader = TextureVertexShader;

}  // namespace eng
