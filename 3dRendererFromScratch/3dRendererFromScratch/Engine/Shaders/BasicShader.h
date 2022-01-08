#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Assets.h"
#include "../Light.h"
#include "../Texture.h"
#include "../InterpolatableTuple.h"
#include "../Primitives.h"

namespace eng {

using VertexShaderUniform = EmptyStruct;

struct FragmentShaderUniform {
    TextureRef diffuseTexture = Assets::getTexture();
    TextureRef normalTexture = Assets::getTexture();
    CubemapTextureRef cubemapTexture = Assets::getCubemapTexture();

    glm::vec3 flatColor = glm::vec3(1.0f);
};

struct WorldSpaceTriangle {
    // vertex coords
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;

    // texture coords
    glm::vec2 t0;
    glm::vec2 t1;
    glm::vec2 t2;

    // tangent, bitangent, normal
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec3 normal;
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

    void setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_);
    BasicVertexShaderOutput run(const WorldSpaceTriangle& tr);

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 viewPos;

    glm::mat4 vm;
    glm::mat3 normalMatrix;
};

}  // namespace eng
