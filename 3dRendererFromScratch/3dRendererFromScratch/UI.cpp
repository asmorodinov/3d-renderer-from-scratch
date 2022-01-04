#include "UI.h"

#include <cstdlib>

UserInterface::UserInterface(sf::RenderWindow& window_) : mainAppWindow_(window_) {
    if (!font_.loadFromFile("data/fonts/arial.ttf")) {
        std::cout << "Failed to load font\n";
        exit(1);
    }

    ImGui::SFML::Init(mainAppWindow_);
    ImGuiIO& io = ImGui::GetIO();
    io.FontDefault = io.Fonts->AddFontFromFileTTF("data/fonts/Roboto-Regular.ttf", 18);
    io.Fonts->Build();
    ImGui::SFML::UpdateFontTexture();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 7.0f;
    style.Alpha = 0.95f;

    text_.setFont(font_);
    text_.setCharacterSize(18);
    text_.setOrigin({0.0f, -22.0f});

    text2_.setFont(font_);
    text2_.setCharacterSize(18);
    text2_.setOrigin({0.0f, -44.0f});

    text3_.setFont(font_);
    text3_.setCharacterSize(18);
    text3_.setOrigin({0.0f, 0.0f});

    text4_.setFont(font_);
    text4_.setCharacterSize(18);
    text4_.setOrigin({0.0f, -66.0f});
}

void UserInterface::updateAndDraw(Seconds deltaTime, size_t trianglesCount, size_t& sceneIndex, std::vector<eng::Scene>& scenes) {
    currentTime_ += deltaTime;
    ++frames_;

    if (currentTime_ - lastTime_ >= 1.5f) {
        framesPerSecond_ = frames_ / (currentTime_ - lastTime_);
        text_.setString("FPS: " + std::to_string(static_cast<int>(framesPerSecond_)));
        text2_.setString("ms per frame: " + std::to_string(static_cast<int>(1000.0f / framesPerSecond_)));
        frames_ = 0;
        lastTime_ = currentTime_;
    }

    text3_.setString("triangles: " + std::to_string(trianglesCount));
    text4_.setString(scenes[sceneIndex].getName());

    mainAppWindow_.get().draw(text_);
    mainAppWindow_.get().draw(text2_);
    mainAppWindow_.get().draw(text3_);
    mainAppWindow_.get().draw(text4_);

    ImGui::SFML::Update(mainAppWindow_, sf::seconds(deltaTime));

    if (pause) {
        auto& scene = scenes[sceneIndex];

        auto& objects = scene.getAllObjects();
        auto& lights = scene.getPointLights();

        ImGui::SetNextWindowSize(ImVec2(650, 700), ImGuiCond_FirstUseEver);
        ImGui::Begin("editor");
        if (ImGui::BeginTabBar("MyTabBar")) {
            if (ImGui::BeginTabItem("Objects")) {
                // Left

                static std::string selectedName;
                static eng::Properties selectedProperties;
                static eng::Properties selectedPropertiesCopy;
                static eng::Properties selectedPropertiesCopy2;
                static eng::Properties selectedPropertiesCopy4;

                static int item_current = 0;
                std::array<const char*, 7> items = {"FlatMesh", "TextureMesh", "CubemapMesh", "UVMesh", "NormalMesh", "PhongMesh", "NormalMapMesh"};

                {
                    ImGui::BeginChild("left", ImVec2(150, 0));
                    ImGui::BeginChild("left pane", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);

                    for (auto& [name, mesh] : objects) {
                        const char* label = name.c_str();
                        if (ImGui::Selectable(label, selectedName == name)) {
                            selectedName = name;
                            selectedProperties = eng::getMeshProperties(objects.at(selectedName), selectedName);
                            selectedPropertiesCopy = selectedProperties;
                            selectedPropertiesCopy2 = selectedProperties;
                            selectedPropertiesCopy4 = selectedProperties;

                            for (int j = 0; j < items.size(); ++j)
                                if (selectedProperties.typeName == items[j]) item_current = j;
                        }
                    }
                    ImGui::EndChild();

                    ImGui::BeginChild("add object", ImVec2(0, 0));
                    if (ImGui::Button("Add object", ImVec2(150, 0))) {
                        for (int i = 1; 1; ++i) {
                            if (objects.count("Mesh" + std::to_string(i)) == 0) {
                                eng::Properties pr;
                                pr.name = "Mesh" + std::to_string(i);
                                eng::addMesh(pr, scene);
                                break;
                            }
                        }
                    }

                    ImGui::EndChild();

                    ImGui::EndChild();
                }
                ImGui::SameLine();

                // Right
                if (objects.count(selectedName) > 0) {
                    bool changedProperty = false;

                    ImGui::BeginGroup();
                    ImGui::BeginChild("item view", ImVec2(0, 0), true);

                    ImGui::InputText("", &selectedPropertiesCopy4.name);
                    ImGui::SameLine();
                    if (ImGui::Button("Rename")) {
                        if (objects.count(selectedPropertiesCopy4.name) == 0) {
                            objects.erase(selectedName);
                            selectedProperties.name = selectedPropertiesCopy4.name;
                            eng::addMesh(selectedProperties, scene);

                            selectedName = selectedPropertiesCopy4.name;
                        }
                    }

                    if (ImGui::Button("Remove object")) {
                        objects.erase(selectedName);
                    }

                    ImGui::Separator();

                    if (ImGui::Combo("Type", &item_current, items.data(), items.size())) {
                        selectedProperties.typeName = items[item_current];
                        changedProperty = true;
                    }

                    ImGui::Separator();

                    ImGui::InputText("meshData file name", &selectedPropertiesCopy.meshFileName);
                    ImGui::DragFloat("meshData scale", &selectedPropertiesCopy.meshScale, 0.01);
                    ImGui::Checkbox("meshData only vertices", &selectedPropertiesCopy.meshOnlyVertices);
                    ImGui::Checkbox("meshData invert normals", &selectedPropertiesCopy.meshInvertNormals);

                    if (ImGui::Button("Edit MeshData")) {
                        selectedProperties.meshFileName = selectedPropertiesCopy.meshFileName;
                        selectedProperties.meshScale = selectedPropertiesCopy.meshScale;
                        selectedProperties.meshOnlyVertices = selectedPropertiesCopy.meshOnlyVertices;
                        selectedProperties.meshInvertNormals = selectedPropertiesCopy.meshInvertNormals;

                        changedProperty = true;
                    }
                    ImGui::Separator();

                    ImGui::InputText("diffuse map name", &selectedPropertiesCopy2.diffuseTextureName);
                    ImGui::InputText("normal map name", &selectedPropertiesCopy2.normalTextureName);
                    ImGui::InputText("cubemap name", &selectedPropertiesCopy2.cubemapTextureName);
                    ImGui::InputText("cubemap image format", &selectedPropertiesCopy2.cubemapImageFormat);
                    ImGui::Checkbox("cubemap default format", &selectedPropertiesCopy2.cubemapDefaultFormat);

                    if (ImGui::Button("Edit textures")) {
                        selectedProperties.diffuseTextureName = selectedPropertiesCopy2.diffuseTextureName;
                        selectedProperties.normalTextureName = selectedPropertiesCopy2.normalTextureName;
                        selectedProperties.cubemapTextureName = selectedPropertiesCopy2.cubemapTextureName;
                        selectedProperties.cubemapImageFormat = selectedPropertiesCopy2.cubemapImageFormat;
                        selectedProperties.cubemapDefaultFormat = selectedPropertiesCopy2.cubemapDefaultFormat;

                        changedProperty = true;
                    }

                    ImGui::Separator();

                    if (ImGui::DragFloat3("position", &selectedProperties.transformPosition.x, 0.01)) changedProperty = true;
                    if (ImGui::DragFloat3("scale", &selectedProperties.transformScale.x, 0.01)) changedProperty = true;

                    if (ImGui::ColorEdit3("flat color", (float*)&selectedProperties.flatColor)) changedProperty = true;
                    if (ImGui::ColorEdit3("wireframe color", (float*)&selectedProperties.wireframeColor)) changedProperty = true;
                    if (ImGui::Checkbox("wireframe mode", &selectedProperties.wireframeMode)) changedProperty = true;
                    if (ImGui::Checkbox("write to depth buffer", &selectedProperties.writeToDepthBuffer)) changedProperty = true;
                    if (ImGui::Checkbox("enable draw", &selectedProperties.drawingEnabled)) changedProperty = true;

                    ImGui::EndChild();

                    ImGui::EndGroup();

                    if (changedProperty) {
                        eng::addMesh(selectedProperties, scene);
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Lights")) {
                // Left

                static size_t selectedLightId = -1;
                static eng::PointLight selectedLight;

                {
                    ImGui::BeginChild("left", ImVec2(150, 0));
                    ImGui::BeginChild("left pane", ImVec2(150, -ImGui::GetFrameHeightWithSpacing()), true);

                    for (size_t i = 0; i < lights.size(); ++i) {
                        std::string name = "light" + std::to_string(i);

                        if (ImGui::Selectable(name.c_str(), i == selectedLightId)) {
                            selectedLightId = i;
                            selectedLight = lights[i];
                        }
                    }
                    ImGui::EndChild();

                    ImGui::BeginChild("add light", ImVec2(0, 0));
                    if (ImGui::Button("Add light", ImVec2(150, 0))) {
                        lights.push_back(eng::PointLight());
                    }

                    ImGui::EndChild();

                    ImGui::EndChild();
                }
                ImGui::SameLine();

                // Right
                if (selectedLightId < lights.size()) {
                    bool changedProperty = false;

                    ImGui::BeginGroup();
                    ImGui::BeginChild("lights view", ImVec2(0, 0), true);

                    if (ImGui::Button("Remove object")) {
                        lights.erase(lights.begin() + selectedLightId);

                        if (selectedLightId < lights.size()) selectedLight = lights[selectedLightId];
                    }

                    ImGui::Separator();

                    if (ImGui::DragFloat3("position", &selectedLight.position.x, 0.01)) changedProperty = true;
                    if (ImGui::ColorEdit3("color", (float*)&selectedLight.color)) changedProperty = true;

                    if (ImGui::DragFloat("intensity", &selectedLight.intensity, 0.005)) changedProperty = true;

                    ImGui::EndChild();

                    ImGui::EndGroup();

                    if (changedProperty) {
                        lights[selectedLightId] = selectedLight;
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Scenes")) {
                // Top
                ImGui::BeginChild("top", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

                // Left
                {
                    ImGui::BeginChild("left", ImVec2(150, 0));
                    ImGui::BeginChild("left panel", ImVec2(150, 0), true);

                    for (size_t i = 0; i < scenes.size(); ++i) {
                        const std::string name = scenes[i].getName() + "##" + std::to_string(i);

                        if (ImGui::Selectable(name.c_str(), i == sceneIndex)) {
                            sceneIndex = i;
                        }
                    }
                    ImGui::EndChild();

                    ImGui::EndChild();
                }
                ImGui::SameLine();

                // Right
                {
                    ImGui::BeginGroup();
                    ImGui::BeginChild("scene view", ImVec2(0, 0), true);

                    static auto currentSceneCopy = scenes[sceneIndex];
                    static auto currentSceneCopy2 = scenes[sceneIndex];
                    static auto lastIndex = sceneIndex;

                    if (lastIndex != sceneIndex) {
                        lastIndex = sceneIndex;
                        currentSceneCopy = scenes[sceneIndex];
                        currentSceneCopy2 = scenes[sceneIndex];
                    }

                    ImGui::InputText("", &currentSceneCopy.getName());
                    ImGui::SameLine();
                    if (ImGui::Button("Rename")) {
                        scenes[sceneIndex].getName() = currentSceneCopy.getName();
                    }

                    ImGui::InputText("##2", &currentSceneCopy2.getName());
                    ImGui::SameLine();
                    if (ImGui::Button("Save as")) {
                        eng::saveSceneToFile(scenes[sceneIndex], currentSceneCopy2.getName());
                    }

                    ImGui::EndChild();
                    ImGui::EndGroup();
                }

                ImGui::EndChild();

                // Bottom
                ImGui::BeginGroup();
                ImGui::BeginChild("load scene", ImVec2(0, 0));
                static auto loadSceneName = std::string("scene");
                ImGui::InputText("", &loadSceneName);
                ImGui::SameLine();

                if (ImGui::Button("load scene", ImVec2(100, 0))) {
                    scenes.push_back(eng::loadSceneFromFile(loadSceneName));
                    sceneIndex = scenes.size() - 1;
                }

                ImGui::EndChild();
                ImGui::EndGroup();

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();

            ImGui::End();
        }
    } else {
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    }

    ImGui::SFML::Render(mainAppWindow_);
}

void UserInterface::togglePause() {
    pause = !pause;
}
