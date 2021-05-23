#pragma once

#include "BasicShader.h"

namespace eng {

struct CubemapShader {
    using Uniform = std::reference_wrapper<const CubemapTexture>;
    using VertexShaderOutput = EmptyStruct;
    using Var = OneVariable<glm::vec3>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights);

    Uniform uniform = Assets::getCubemapTexture();
    VertexShaderOutput vso;
};

struct CubemapVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<CubemapShader::Var> triangle;
        CubemapShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    Output run(const WorldSpaceTriangle& tr);

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
