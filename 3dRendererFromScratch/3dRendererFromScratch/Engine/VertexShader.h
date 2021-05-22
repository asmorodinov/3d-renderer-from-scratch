#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common.h"
#include "Light.h"
#include "Texture.h"
#include "HelperClasses.h"

#include "Shader.h"
#include "Primitives.h"

namespace eng {

struct WorldSpaceTriangle {
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;

    glm::vec2 t0;
    glm::vec2 t1;
    glm::vec2 t2;

    glm::vec3 n0;
    glm::vec3 n1;
    glm::vec3 n2;
};

struct BasicVertexShader {
    struct BasicVertexShaderOutput {
        // world space
        glm::vec3 v0;
        glm::vec3 v1;
        glm::vec3 v2;

        // camera space
        glm::vec4 cv0;
        glm::vec4 cv1;
        glm::vec4 cv2;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        model = model_;
        view = view_;
        projection = projection_;

        viewPos = viewPos_;

        vm = view * model;

        normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    }

    BasicVertexShaderOutput run(const WorldSpaceTriangle& tr) {
        glm::vec4 v0 = glm::vec4(tr.v0, 1.0f);
        glm::vec4 v1 = glm::vec4(tr.v1, 1.0f);
        glm::vec4 v2 = glm::vec4(tr.v2, 1.0f);

        glm::vec4 p0_ = model * v0;
        glm::vec4 p1_ = model * v1;
        glm::vec4 p2_ = model * v2;
        p0_ /= p0_.w;
        p1_ /= p1_.w;
        p2_ /= p2_.w;
        glm::vec3 p0 = glm::vec3(p0_);
        glm::vec3 p1 = glm::vec3(p1_);
        glm::vec3 p2 = glm::vec3(p2_);

        p0_ = vm * v0;
        p1_ = vm * v1;
        p2_ = vm * v2;
        glm::vec4 tp0 = p0_ / p0_.w;
        glm::vec4 tp1 = p1_ / p1_.w;
        glm::vec4 tp2 = p2_ / p2_.w;

        return {p0, p1, p2, tp0, tp1, tp2};
    }

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 viewPos;

    glm::mat4 vm;
    glm::mat3 normalMatrix;
};

// flat vertex shader
struct FlatVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<FlatShader::Var> triangle;
        FlatShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, {}, {}, {}}, {}};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

// texture vertex shader
struct TextureVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<TextureShader::Var> triangle;
        TextureShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, tr.t0, tr.t1, tr.t2}, {}};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

// cubemap vertex shader
struct CubemapVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<CubemapShader::Var> triangle;
        CubemapShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
        bvs.view = glm::mat4(glm::mat3(bvs.view));
        bvs.vm = bvs.view * bvs.model;
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0}, {bvso.v1}, {bvso.v2}}, {}};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

// uv vertex shader
using UVVertexShader = TextureVertexShader;

// normal vertex shader
struct NormalVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<NormalShader::Var> triangle;
        NormalShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, {}, {}, {}}, {bvs.normalMatrix * tr.n0}};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

// phong vertex shader
struct PhongVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<PhongShader::Var> triangle;
        PhongShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0, tr.t0}, {bvso.v1, tr.t1}, {bvso.v2, tr.t2}},
                {bvs.viewPos, bvs.normalMatrix * tr.n0}};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

// normal map vertex shader
struct NormalMapVertexShader {
    using Uniform = EmptyStruct;
    struct Output {
        Triangle<NormalMapShader::Var> triangle;
        NormalMapShader::VertexShaderOutput uniformOutput;
    };

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
        bvs.setMVP(model_, view_, projection_, viewPos_);
    }
    Output run(const WorldSpaceTriangle& tr) {
        auto bvso = bvs.run(tr);
        return {{bvso.cv0, bvso.cv1, bvso.cv2, {bvso.v0, tr.t0}, {bvso.v1, tr.t1}, {bvso.v2, tr.t2}}, bvs.viewPos};
    }

    Uniform uniform;
    BasicVertexShader bvs;
};

}  // namespace eng
