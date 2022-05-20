#pragma once

#include "BasicShader.h"

namespace eng {

struct PhongShadowMappingShader {
    using Uniform = FragmentShaderUniform;
    struct VertexShaderOutput {
        glm::vec3 viewPos;
        glm::vec3 normal;
    };
    using Var = ThreeVariables<glm::vec4, glm::vec3, glm::vec2>;  // fragPos lightspace, fragPos, uv

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct PhongShadowMappingVertexShader {
    using Uniform = VertexShaderUniform;
    struct Output {
        Triangle<PhongShadowMappingShader::Var> triangle;
        PhongShadowMappingShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
