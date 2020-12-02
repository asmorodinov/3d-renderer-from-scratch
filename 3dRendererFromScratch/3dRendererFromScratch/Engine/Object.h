#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
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

 private:
    ColorType color;
    unsigned pixelRadius;
};

class Line : public Object {
 public:
    Line(glm::vec3 pos1, glm::vec3 pos2, ColorType color = {1.0f, 1.0f, 1.0f});

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2;
};

class TriangleObj : public Object {
 public:
    TriangleObj(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color, PhongShader& ph, FlatShader& fl, bool s = false,
                bool flat = false);

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) override;
    void update(float dt) override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2, pos3;
    bool s, flat;

    PhongShader& ph;
    FlatShader& fl;

    PhongTriangle pht;
    FlatTriangle flt;
};

}  // namespace eng
