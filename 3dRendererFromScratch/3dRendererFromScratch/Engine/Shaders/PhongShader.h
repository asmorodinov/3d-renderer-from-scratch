#pragma once

#include "BasicShader.h"

namespace eng {

struct PhongShader {
    using Uniform = std::reference_wrapper<const Texture>;
    struct VertexShaderOutput {
        glm::vec3 viewPos;
        glm::vec3 normal;
    };
    using Var = TwoVariables<glm::vec3, glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform = Assets::getTexture();
    VertexShaderOutput vso;
};

struct PhongVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<PhongShader::Var> triangle;
        PhongShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
