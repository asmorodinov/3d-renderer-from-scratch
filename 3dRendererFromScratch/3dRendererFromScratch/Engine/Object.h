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

    Transform& getTransform() { return t; }

 protected:
    Transform t;
};

using ObjectPtr = std::unique_ptr<Object>;

class Point : public Object {
 public:
    Point(glm::vec3 position, ColorType color = {1.0f, 1.0f, 1.0f}, unsigned pixelRadius = 1)
        : color(color), pixelRadius(pixelRadius) {
        t.position = position;
    }

    void draw(const Camera& camera, Screen& screen) override {
        glm::vec2 screenCoords = LocalCoordsToScreenCoords(camera.getViewMatrix(), screen.getProjectionMatrix(),
                                                           t.getModel(), glm::vec3(0.0f));

        if (screenCoords.x <= -1.0f || screenCoords.x >= 1.0f || screenCoords.y <= -1.0f || screenCoords.y >= 1.0f)
            return;

        int x = int(screen.getWidth() * (screenCoords.x + 1.0f) / 2.0f);
        int y = int(screen.getHeight() * (screenCoords.y + 1.0f) / 2.0f);

        for (int i = x - pixelRadius; i <= x + pixelRadius; ++i)
            for (int j = y - pixelRadius; j <= y + pixelRadius; ++j) {
                if (i < 0 || j < 0) continue;

                screen.setPixelColor(size_t(i), size_t(j), color);
            }
    }
    void update(float dt) override{};

 private:
    ColorType color;
    unsigned pixelRadius;
};

}  // namespace eng
