#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Texture.h"

namespace eng {

// helper classes
struct EmptyStruct {};
struct NoVariables {
    void operator*=(float t) {}
    NoVariables operator*(float t) const { return {}; }
    NoVariables operator+(const NoVariables& oth) const { return {}; }
};
template <typename T>
struct OneVariable {
    void operator*=(float t_) { t *= t_; }
    OneVariable operator*(float t_) const { return {t * t_}; }
    OneVariable operator+(const OneVariable& oth) const { return {t + oth.t}; }

    T t;
};
template <typename T1, typename T2>
struct TwoVariables {
    void operator*=(float t) {
        t1 *= t;
        t2 *= t;
    }
    TwoVariables operator*(float t) const { return {t1 * t, t2 * t}; }
    TwoVariables operator+(const TwoVariables& oth) const { return {t1 + oth.t1, t2 + oth.t2}; }
    T1 t1;
    T2 t2;
};

// Flat shader
using FlatShaderConst = glm::vec3;
using FlatShaderVar = NoVariables;
struct FlatShader {
    glm::vec4 computePixelColor(const FlatShaderVar& var, const LightsVec& lights) { return glm::vec4(c, 1.0f); }
    FlatShaderConst c;
};

// Texture shader
using TextureShaderConst = std::shared_ptr<Texture>;
using TextureShaderVar = OneVariable<glm::vec2>;
struct TextureShader {
    glm::vec4 computePixelColor(const TextureShaderVar& var, const LightsVec& lights) { return c->sample(var.t.x, var.t.y); }
    TextureShaderConst c;
};

// Cubemap shader
using CubemapShaderConst = std::shared_ptr<CubemapTexture>;
using CubemapShaderVar = OneVariable<glm::vec3>;
struct CubemapShader {
    glm::vec4 computePixelColor(const CubemapShaderVar& var, const LightsVec& lights) { return c->sample(var.t); }
    CubemapShaderConst c;
};

// UV shader
using UVShaderConst = EmptyStruct;
using UVShaderVar = OneVariable<glm::vec2>;
struct UVShader {
    glm::vec4 computePixelColor(const UVShaderVar& var, const LightsVec& lights) {
        return glm::vec4(var.t.x, var.t.y, 0.5f, 1.0f);
    }
    UVShaderConst c;
};

// Normal shader
using NormalShaderConst = glm::vec3;
using NormalShaderVar = NoVariables;
struct NormalShader {
    glm::vec4 computePixelColor(const NormalShaderVar& var, const LightsVec& lights) {
        return glm::vec4(0.5f * (c + 1.0f), 1.0f);
    }
    NormalShaderConst c;
};

// Phong shader
struct PhongShaderConst {
    std::shared_ptr<Texture> texture;
    std::shared_ptr<CubemapTexture> skybox;
    glm::vec3 viewPos;
    glm::vec3 normal;
};
using PhongShaderVar = TwoVariables<glm::vec3, glm::vec2>;
struct PhongShader {
    glm::vec4 computePixelColor(const PhongShaderVar& var, const LightsVec& lights) {
        auto FragPos = var.t1;
        auto uv = var.t2;

        auto texture = c.texture;
        auto viewPos = c.viewPos;
        auto normal = c.normal;
        auto skybox = c.skybox;

        glm::vec4 color = texture->sample(uv.s, uv.t);

        glm::vec3 lighting = glm::vec3(0.0f);

        glm::vec3 I = glm::normalize(FragPos - viewPos);
        glm::vec3 R = glm::reflect(I, glm::normalize(normal));

        lighting = skybox->sample(R);
        lighting += 0.2f * glm::vec3(color);

        for (const auto& light : lights) {
            glm::vec3 lightPos = light.pos;
            glm::vec3 viewDir = glm::normalize(viewPos - FragPos);
            glm::vec3 lightDir = glm::normalize(lightPos - FragPos);

            float diff = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.2f);
            glm::vec3 diffuse = diff * glm::vec3(color) * light.color * light.diff;

            glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
            float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
            glm::vec3 specular = light.color * spec * light.spec;

            float d = length(lightPos - FragPos);
            float attenuation = 1.0f / (1.0f + d * light.lin + std::pow(d, 2.0f) * light.quad + std::pow(d, 3.0f) * light.cube);
            diffuse *= attenuation / 1.7;
            specular *= attenuation / 1.7;
            lighting += 0.3f * light.intensity * glm::vec3(1.0f) * (diffuse + specular);
        }

        lighting = glm::pow(lighting, glm::vec3(1.0f / 2.2f));

        return glm::vec4(lighting, 1.0f);
    }

    PhongShaderConst c;
};

}  // namespace eng
