#pragma once

#include "BasicShader.h"

namespace eng {

struct FlatShader {
    using Uniform = glm::vec3;
    using VertexShaderOutput = EmptyStruct;
    using Var = NoVariables;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct FlatVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<FlatShader::Var> triangle;
        FlatShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
