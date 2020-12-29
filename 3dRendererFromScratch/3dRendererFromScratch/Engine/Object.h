#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Common.h"

#include "Shader.h"

namespace eng {

class Object {
 public:
    virtual ~Object() = default;

    virtual void draw(const Camera& camera, Screen& screen, const std::vector<PointLight>& lights) = 0;
    virtual void update(float dt) = 0;
    virtual size_t getTriangleCount() = 0;

    Transform& getTransform();

 protected:
    Transform t;
};

using ObjectPtr = std::unique_ptr<Object>;
using ObjectsVec = std::vector<ObjectPtr>;

class Point : public Object {
 public:
    Point(glm::vec3 position, ColorType color = {1.0f, 1.0f, 1.0f}, unsigned pixelRadius = 1);

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

 private:
    ColorType color;
    unsigned pixelRadius;
};

class Line : public Object {
 public:
    Line(glm::vec3 pos1, glm::vec3 pos2, ColorType color = {1.0f, 1.0f, 1.0f});

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2;
};

enum struct RenderMode { FlatColor, Texture, Phong };

class TriangleObj : public Object {
 public:
    TriangleObj(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color, PhongShader& ph, FlatShader& fl,
                TextureShader& td, bool s = false, RenderMode rm = RenderMode::FlatColor);

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2, pos3;
    bool s;
    RenderMode rm;

    PhongShader& ph;
    FlatShader& fl;
    TextureShader& td;

    PhongTriangle pht;
    FlatTriangle flt;
    TexturedTriangle tdt;
};

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
    Mesh(const MeshData& mesh, ColorType color, PhongShader& ph, FlatShader& fl, TextureShader& td,
         RenderMode rm = RenderMode::Texture);

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;
    size_t getTriangleCount() override;

 private:
    MeshData mesh;
    ColorType color;
    RenderMode rm;

    PhongShader& ph;
    FlatShader& fl;
    TextureShader& td;
};

}  // namespace eng
