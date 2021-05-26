#include "ObjectTransform.h"

namespace eng {

ObjectTransform::ObjectTransform(glm::vec3 position) : position_(position) {
}

glm::mat4 ObjectTransform::getModel() {
    if (modelInvalid_) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 rotationMatrix = glm::mat4_cast(orientation_);

        model = glm::translate(model, position_);
        model = model * rotationMatrix;
        model = glm::scale(model, scale_);

        model_ = model;
        modelInvalid_ = false;
    }

    return model_;
}

glm::vec3 ObjectTransform::getPosition() const {
    return position_;
}

glm::quat ObjectTransform::getOrientation() const {
    return orientation_;
}

glm::vec3 ObjectTransform::getScale() const {
    return scale_;
}

void ObjectTransform::setPosition(const glm::vec3 position) {
    position_ = position;
    modelInvalid_ = true;
}

void ObjectTransform::setOrientation(const glm::quat orientation) {
    orientation_ = orientation;
    modelInvalid_ = true;
}

void ObjectTransform::setScale(const glm::vec3 scale) {
    scale_ = scale;
    modelInvalid_ = true;
}

}  // namespace eng
