#pragma once

#include "BasicShader.h"

namespace eng {

struct ParallaxMapShader {
    using Uniform = FragmentShaderUniform;
    struct VertexShaderOutput {
        glm::vec3 viewPos;
        glm::mat3 TBN;
    };

    using Var = TwoVariables<glm::vec3, glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct ParallaxMapVertexShader {
    using Uniform = VertexShaderUniform;
    struct Output {
        Triangle<ParallaxMapShader::Var> triangle;
        ParallaxMapShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
