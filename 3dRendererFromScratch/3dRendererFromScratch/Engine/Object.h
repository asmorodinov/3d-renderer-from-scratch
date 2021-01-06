#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>

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

    virtual void draw(RenderMode r, const Camera& camera, Screen& screen, const std::vector<PointLight>& lights) = 0;
    virtual void update(float dt) = 0;
    virtual size_t getTriangleCount() = 0;

    Transform& getTransform();

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
};

MeshData loadFromObj(const std::string& filename, float scale = 1.0f, bool invertNormals = false, bool uvCoords = true);

class Mesh : public Object {
 public:
    Mesh(const MeshData& mesh, Texture* texture, ColorType color, Shader& ph, Shader& fl, Shader& td, Shader& uv, Shader& nrm,
         std::optional<RenderMode> rm = std::nullopt);

    void draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

    void setColor(ColorType clr);
    ColorType getColor() const;

    void setPosition(glm::vec3 pos);
    glm::vec3 getPosition() const;

 private:
    MeshData mesh;
    ColorType color;
    Texture* texture;
    std::optional<RenderMode> rm;

    Shader& ph;
    Shader& fl;
    Shader& td;
    Shader& uv;
    Shader& nrm;
};

}  // namespace eng
