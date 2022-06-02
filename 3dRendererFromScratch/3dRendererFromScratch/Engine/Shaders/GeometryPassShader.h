#pragma once

#include "BasicShader.h"

namespace eng {

struct GeometryInfo {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 diffuseColor = glm::vec3(0.0f);
};

struct GeometryPassShader {
    using Uniform = FragmentShaderUniform;
    struct VertexShaderOutput {
        glm::vec3 normal;
    };
    using Var = TwoVariables<glm::vec3, glm::vec2>;

    GeometryInfo computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct GeometryPassVertexShader {
    using Uniform = VertexShaderUniform;
    struct Output {
        Triangle<GeometryPassShader::Var> triangle;
        GeometryPassShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
