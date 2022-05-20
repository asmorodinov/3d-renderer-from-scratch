#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Types.h"

namespace eng {

class ProjectionInfo {
 private:
    static constexpr const Depth defaultNearPlaneDistance_ = 0.2f;
    static constexpr const Depth defaultFarPlaneDistance_ = 10.0f;
    static constexpr const Degrees defaultFieldOfViewAngle_ = 60.0f;

 public:
    ProjectionInfo(Pixels width, Pixels height);
    const glm::mat4& getProjectionMatrix() const;

    Pixels getWidth() const;
    Pixels getHeight() const;

    Depth getNearPlaneDistance() const;
    Depth getFarPlaneDistance() const;

 private:
    Depth nearPlaneDistance_, farPlaneDistance_;

    Pixels screenWidth_, screenHeight_;
    AspectRatio aspectRatio_ = 1.0f;

    glm::mat4 projectionMatrix_;
};

}  // namespace eng
