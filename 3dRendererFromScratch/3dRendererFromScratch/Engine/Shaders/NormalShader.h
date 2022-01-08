#pragma once

#include "BasicShader.h"

namespace eng {

struct NormalShader {
    using Uniform = FragmentShaderUniform;
    using VertexShaderOutput = glm::vec3;
    using Var = NoVariables;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct NormalVertexShader {
    using Uniform = VertexShaderUniform;
    struct Output {
        Triangle<NormalShader::Var> triangle;
        NormalShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
