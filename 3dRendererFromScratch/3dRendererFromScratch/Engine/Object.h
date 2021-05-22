#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Common.h"

#include "Shader.h"

namespace eng {

enum struct RenderMode { Wireframe, FlatColor, UV, Normals, Texture, Phong };

class Object {
 public:
    virtual ~Object() = default;

    virtual void draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) = 0;
    virtual void update(float dt) = 0;
    virtual size_t getTriangleCount() = 0;

    Transform& getTransform();

    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition() const;

 protected:
    Transform t;
};

using ObjectPtr = std::unique_ptr<Object>;
using ObjectsVec = std::vector<ObjectPtr>;

struct Face {
    // vertex indices
    int i, j, k;
    // texture indices
    int ti, tj, tk;
};

struct MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> textureCoords;
    std::vector<Face> faces;

    static MeshData generateCubeData(float sz = 1.0f, bool invertNormals = false);
};

MeshData loadFromObj(const std::string& filename, float scale = 1.0f, bool invertNormals = false, bool uvCoords = true);

class Mesh : public Object {
 public:
    Mesh(const MeshData& mesh, std::shared_ptr<Texture> texture, ColorType color, PhongShader& ph, FlatShader& fl,
         TextureShader& td, UVShader& uv, NormalShader& nrm, NormalMapShader& nrmm, std::optional<RenderMode> rm = std::nullopt,
         std::optional<std::shared_ptr<Texture>> nrmmap = std::nullopt);

    void draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

    void setColor(ColorType clr);
    ColorType getColor() const;

 private:
    MeshData mesh;
    ColorType color;
    std::shared_ptr<Texture> texture;
    std::optional<std::shared_ptr<Texture>> normalMap;
    std::optional<RenderMode> rm;

    PhongShader& ph;
    FlatShader& fl;
    TextureShader& td;
    UVShader& uv;
    NormalShader& nrm;
    NormalMapShader& nrmm;
};

class Skybox : public Object {
 public:
    Skybox(const MeshData& mesh, CubemapShader& shader, std::optional<RenderMode> rm = std::nullopt);

    void draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

 private:
    MeshData mesh;
    CubemapShader& shader;

    std::optional<RenderMode> rm;
};

}  // namespace eng
