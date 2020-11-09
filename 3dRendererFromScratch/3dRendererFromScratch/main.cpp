#include "Engine/Renderer.h"

#include "profile.h"

int main() {
    eng::Renderer renderer(640, 480);

    {
        LOG_DURATION("prepare scene");

        float scale = 0.2f;
        for (float i = -1; i <= 1; i += 0.2)
            for (float j = -1; j <= 1; j += 0.2)
                for (float k = -1; k <= 1; k += 0.2) {
                    auto p = std::make_unique<eng::Point>(
                        glm::vec3(scale * i, scale * j, scale * k),
                        eng::ColorType{(i + 1.0f) * 0.5f, (j + 1.0f) * 0.5f, (k + 1.0f) * 0.5f}, 2);

                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(0, 0, 10));
                    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 1, 1));

                    p->getTransform().extraTransform = model;

                    renderer.getWorld().getObjects().emplace_back(std::move(p));
                }
    }

    {
        LOG_DURATION("render scene to screen");
        renderer.renderSceneToScreen();
    }

    {
        LOG_DURATION("save image");
        renderer.renderScreenToFile("image.png");
    }

    return 0;
}
