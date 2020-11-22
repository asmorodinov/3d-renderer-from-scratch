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

namespace eng {

class Object {
 public:
    virtual ~Object() = default;

    virtual void draw(const Camera& camera, Screen& screen) = 0;
    virtual void update(float dt) = 0;

    Transform& getTransform();

 protected:
    Transform t;
};

using ObjectPtr = std::unique_ptr<Object>;

class Point : public Object {
 public:
    Point(glm::vec3 position, ColorType color = {1.0f, 1.0f, 1.0f}, unsigned pixelRadius = 1);

    void draw(const Camera& camera, Screen& screen) override;
    void update(float dt) override;

 private:
    ColorType color;
    unsigned pixelRadius;
};

class Line : public Object {
 public:
    Line(glm::vec3 pos1, glm::vec3 pos2, ColorType color = {1.0f, 1.0f, 1.0f});

    void draw(const Camera& camera, Screen& screen) override;
    void update(float dt) override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2;
};

class Triangle : public Object {
 public:
    Triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, ColorType color = {1.0f, 1.0f, 1.0f}, bool s = false);

    void draw(const Camera& camera, Screen& screen) override;
    void update(float dt) override;

 private:
    ColorType color;
    glm::vec3 pos1, pos2, pos3;
    bool s;
    static Texture texture;
};

}  // namespace eng
