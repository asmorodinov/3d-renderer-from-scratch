#pragma once

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Light.h"
#include "Texture.h"
#include "HelperClasses.h"

namespace eng {
// Flat shader
struct FlatShader {
    using Const = glm::vec3;
    using Var = NoVariables;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(c, 1.0f); }
    Const c;
};

// Texture shader
struct TextureShader {
    using Const = std::shared_ptr<Texture>;
    using Var = OneVariable<glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) { return c->sample(var.t.x, var.t.y); }
    Const c;
};

// Cubemap shader
struct CubemapShader {
    using Const = std::shared_ptr<CubemapTexture>;
    using Var = OneVariable<glm::vec3>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) { return c->sample(var.t); }
    Const c;
};

// UV shader
struct UVShader {
    using Const = EmptyStruct;
    using Var = OneVariable<glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(var.t.x, var.t.y, 0.5f, 1.0f); }
    Const c;
};

// Normal shader
struct NormalShader {
    using Const = glm::vec3;
    using Var = NoVariables;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) { return glm::vec4(0.5f * (c + 1.0f), 1.0f); }
    Const c;
};

// Phong shader
struct PhongShader {
    struct Const {
        std::shared_ptr<Texture> texture;
        std::shared_ptr<CubemapTexture> skybox;
        glm::vec3 viewPos;
        glm::vec3 normal;
    };
    using Var = TwoVariables<glm::vec3, glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) {
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

    Const c;
};

// Normal Map shader
struct NormalMapShader {
    struct Const {
        std::shared_ptr<Texture> diffuseMap;
        std::shared_ptr<Texture> normalMap;
        std::shared_ptr<CubemapTexture> skybox;
        glm::vec3 viewPos;
    };
    using Var = TwoVariables<glm::vec3, glm::vec2>;

    glm::vec4 computePixelColor(const Var& var, const LightsVec& lights) {
        auto FragPos = var.t1;
        auto uv = var.t2;

        auto diffuseMap = c.diffuseMap;
        auto normalMap = c.normalMap;
        auto viewPos = c.viewPos;
        auto normal = glm::normalize(glm::vec3(normalMap->sample(uv.s, uv.t)) * 2.0f - 1.0f);

        glm::vec4 color = diffuseMap->sample(uv.s, uv.t);

        glm::vec3 lighting = glm::vec3(0.0f);

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
            lighting += 1.0f * light.intensity * glm::vec3(1.0f) * (diffuse + specular);
        }

        lighting = glm::pow(lighting, glm::vec3(1.0f / 2.2f));

        return glm::vec4(lighting, 1.0f);
    }

    Const c;
};

}  // namespace eng
