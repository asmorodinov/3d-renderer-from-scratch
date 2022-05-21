#include "ProjectionInfo.h"

namespace eng {

ProjectionInfo::ProjectionInfo(Pixels width, Pixels height, Depth near, Depth far, Degrees fov)
    : nearPlaneDistance_(near),
      farPlaneDistance_(far),
      screenWidth_(width),
      screenHeight_(height),
      aspectRatio_(width / (float)height),
      projectionMatrix_(glm::perspective(glm::radians(fov), aspectRatio_, nearPlaneDistance_, farPlaneDistance_)) {
}

ProjectionInfo::ProjectionInfo(Pixels width, Pixels height, bool dummy, Distance frustrumSize, Depth near, Depth far)
    : nearPlaneDistance_(near),
      farPlaneDistance_(far),
      screenWidth_(width),
      screenHeight_(height),
      aspectRatio_(width / (float)height),
      projectionMatrix_(glm::ortho(-frustrumSize, frustrumSize, -frustrumSize, frustrumSize, near, far)) {
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
