#pragma once

#include "BasicShader.h"

namespace eng {

struct TextureShader {
    using Uniform = FragmentShaderUniform;
    using VertexShaderOutput = EmptyStruct;
    using Var = OneVariable<glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct TextureVertexShader {
    using Uniform = VertexShaderUniform;
    struct Output {
        Triangle<TextureShader::Var> triangle;
        TextureShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
