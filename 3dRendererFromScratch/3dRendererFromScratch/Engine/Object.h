#pragma once

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

}  // namespace eng
