#include "Common.h"

namespace eng {

glm::mat4 Transform::getModel() const {
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 rotationMatrix = glm::mat4_cast(orientation);

    model = extraTransform;
    model = glm::translate(model, position);
    model = model * rotationMatrix;
    model = glm::scale(model, scale);

    return model;
}

glm::vec2 LocalCoordsToScreenCoords(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
                                    const glm::mat4& objectModel, glm::vec3 localCoords) {
    glm::vec4 worldCoords = objectModel * glm::vec4(localCoords, 1.0f);

    glm::vec2 screenCoords = projectionMatrix * viewMatrix * worldCoords;

    return screenCoords;
}

}  // namespace eng
