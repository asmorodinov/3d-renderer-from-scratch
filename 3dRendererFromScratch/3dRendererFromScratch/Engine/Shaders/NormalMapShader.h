#pragma once

#include "BasicShader.h"

namespace eng {

struct NormalMapShader {
    struct Uniform {
        std::reference_wrapper<const Texture> diffuseMap = Assets::getTexture();
        std::reference_wrapper<const Texture> normalMap = Assets::getTexture();
    };
    struct VertexShaderOutput {
        glm::vec3 viewPos;
        glm::mat3 invTBN;
    };

    using Var = TwoVariables<glm::vec3, glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform;
    VertexShaderOutput vso;
};

struct NormalMapVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<NormalMapShader::Var> triangle;
        NormalMapShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
