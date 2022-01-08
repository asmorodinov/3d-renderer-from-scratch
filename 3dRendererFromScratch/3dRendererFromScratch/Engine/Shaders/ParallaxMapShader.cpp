#include "ParallaxMapShader.h"

namespace eng {

static constexpr auto heightScale = 0.1f;

// parallax mapping helper function
glm::vec2 ParallaxMapping(glm::vec2 texCoords, glm::vec3 viewDir, const Texture& depthMap) {
    using namespace glm;

    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = glm::vec2(viewDir.x, viewDir.y) / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    // get initial values
    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = depthMap.sample(currentTexCoords).r;

    while (currentLayerDepth < currentDepthMapValue) {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = depthMap.sample(currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = depthMap.sample(prevTexCoords).r - currentLayerDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

    return finalTexCoords;
}

glm::vec4 ParallaxMapShader::computePixelColor(const Var& var, const LightsVec& lights) {
    auto FragPos = var.t1;
    auto uv = var.t2;

    // clip tex coords
    // uv = glm::vec2(uv.x - glm::floor(uv.x), uv.y - glm::floor(uv.y));

    auto diffuseMap = uniform.diffuseTexture;
    auto normalMap = uniform.normalTexture;
    auto viewPos = vso.viewPos;

    auto viewDir = glm::normalize(viewPos - FragPos);

    uv = ParallaxMapping(uv, viewDir, uniform.displacementTexture.get());

    // if (false)
    if (uv.x > 1.0f || uv.y > 1.0f || uv.x < 0.0f || uv.y < 0.0f) {
        return glm::vec4();
    }

    glm::vec4 color = diffuseMap.get().sample(uv);
    auto normal = glm::normalize(glm::vec3(normalMap.get().sample(uv)) * 2.0f - 1.0f);

    // return color;

    glm::vec3 lighting = glm::vec3(0.0f);

    for (const auto& light : lights) {
        glm::vec3 lightPos = vso.TBN * light.position;

        glm::vec3 lightDir = glm::normalize(lightPos - FragPos);

        float diff = (glm::max(glm::dot(normal, lightDir), 0.0f) + 0.1f);
        glm::vec3 diffuse = diff * glm::vec3(color) * light.color * light.diffuseCoefficient;

        glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir);
        float spec = glm::pow(glm::max(glm::dot(normal, halfwayDir), 0.0f), 32.0f);
        glm::vec3 specular = light.color * spec * light.specularCoefficient;

        float d = length(lightPos - FragPos);
        float attenuation = 1.0f / (1.0f + d * light.linearAttenuationCoefficient + std::pow(d, 2.0f) * light.quadraticAttenuationCoefficient +
                                    std::pow(d, 3.0f) * light.cubicAttenuationCoefficient);
        diffuse *= attenuation / 1.7;
        specular *= attenuation / 1.7;

        lighting += light.intensity * (diffuse + specular);
    }

    lighting = glm::clamp(glm::pow(lighting, glm::vec3(1.0f / 2.2f)), 0.0f, 1.0f);

    return glm::vec4(lighting, 1.0f);
}

void ParallaxMapVertexShader::setMVP(glm::mat4 model_, glm::mat4 view_, glm::mat4 projection_, glm::vec3 viewPos_) {
    bvs.setMVP(model_, view_, projection_, viewPos_);
}

ParallaxMapVertexShader::Output ParallaxMapVertexShader::run(const WorldSpaceTriangle& tr) {
    auto bvso = bvs.run(tr);

    auto T = glm::normalize(bvs.normalMatrix * tr.tangent);
    auto N = glm::normalize(bvs.normalMatrix * tr.normal);
    T = glm::normalize(T - glm::dot(T, N) * N);
    auto B = glm::cross(N, T);

    glm::mat3 TBN = glm::transpose(glm::mat3(T, B, N));

    return {{bvso.cv0, bvso.cv1, bvso.cv2, {TBN * bvso.v0, tr.t0}, {TBN * bvso.v1, tr.t1}, {TBN * bvso.v2, tr.t2}}, {TBN * bvs.viewPos, TBN}};
}

}  // namespace eng
