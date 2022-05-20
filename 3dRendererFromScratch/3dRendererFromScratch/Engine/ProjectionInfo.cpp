#include "ProjectionInfo.h"

namespace eng {

ProjectionInfo::ProjectionInfo(Pixels width, Pixels height)
    : nearPlaneDistance_(defaultNearPlaneDistance_),
      farPlaneDistance_(defaultFarPlaneDistance_),
      screenWidth_(width),
      screenHeight_(height),
      aspectRatio_(width / (float)height),
      projectionMatrix_(glm::perspective(glm::radians(defaultFieldOfViewAngle_), aspectRatio_, nearPlaneDistance_, farPlaneDistance_)) {
}

const glm::mat4& ProjectionInfo::getProjectionMatrix() const {
    return projectionMatrix_;
}

Pixels ProjectionInfo::getWidth() const {
    return screenWidth_;
}

Pixels ProjectionInfo::getHeight() const {
    return screenHeight_;
}

Depth ProjectionInfo::getNearPlaneDistance() const {
    return nearPlaneDistance_;
}

Depth ProjectionInfo::getFarPlaneDistance() const {
    return farPlaneDistance_;
}

}  // namespace eng
