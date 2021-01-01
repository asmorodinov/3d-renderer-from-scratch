#include "Common.h"

namespace eng {

glm::mat4 Transform::getModel() const {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 rotationMatrix = glm::mat4_cast(orientation);

    model = glm::translate(model, position);
    model = model * rotationMatrix;
    model = glm::scale(model, scale);

    return model;
}

}  // namespace eng
