#pragma once

#include <functional>
#include <algorithm>
#include <climits>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Camera.h"
#include "Screen.h"
#include "Light.h"

#include "MeshData.h"
#include "ObjectTransform.h"

namespace eng {

template <typename VertexShader, typename FragmentShader>
class Mesh {
 public:
    using MeshDataRef = std::reference_wrapper<const MeshData>;
    using VertexShaderUniform = typename VertexShader::Uniform;
    using FragmentShaderUniform = typename FragmentShader::Uniform;
    using VertexShaderOutput = typename VertexShader::Output;

    Mesh(MeshDataRef mesh, const VertexShaderUniform& vertexShaderUniform, const FragmentShaderUniform& fragmentShaderUniform,
         const ObjectTransform& objectTransform, bool wireframeMode = false, bool writeToDepthBuffer = true, glm::vec3 wireframeColor = glm::vec3(1.0f),
         bool drawingEnabled = true)
        : mesh_(mesh),
          vertexShaderUniform_(vertexShaderUniform),
          fragmentShaderUniform_(fragmentShaderUniform),
          objectTransform_(objectTransform),
          vertexShader_(),
          fragmentShader_(),
          wireframeMode_(wireframeMode),
          writeToDepthBuffer_(writeToDepthBuffer),
          drawingEnabled_(drawingEnabled),
          wireframeColor_(wireframeColor) {
    }

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
        if (!drawingEnabled_) return;

        const auto& model = objectTransform_.getModel();
        const auto& view = camera.getViewMatrix();
        const auto& projection = screen.getProjectionMatrix();
        const auto& viewPos = camera.getPosition();

        vertexShader_.uniform = vertexShaderUniform_;
        fragmentShader_.uniform = fragmentShaderUniform_;

        vertexShader_.setMVP(model, view, projection, viewPos);

        const auto& mesh = mesh_.get();

        for (const auto& face : mesh.faces) {
            VertexShaderOutput out = vertexShader_.run({mesh.vertices[face.i], mesh.vertices[face.j], mesh.vertices[face.k], mesh.textureCoords[face.ti],
                                                        mesh.textureCoords[face.tj], mesh.textureCoords[face.tk], mesh.normals[face.ni], mesh.normals[face.nj],
                                                        mesh.normals[face.nk]});
            const auto& triangle = out.triangle;
            fragmentShader_.vso = out.uniformOutput;

            if (wireframeMode_) {
                drawWireframeTriangle({triangle.p0, triangle.p1, triangle.p2, {}, {}, {}}, projection, wireframeColor_, screen);
            } else {
                if (!writeToDepthBuffer_) screen.setWriteToDepthBuffer(false);

                drawTriangle(triangle, projection, fragmentShader_, screen, lights);

                if (!writeToDepthBuffer_) screen.setWriteToDepthBuffer(true);
            }
        }
    }

    void setVertexShaderUniform(const VertexShaderUniform& vertexShaderUniform) {
        vertexShaderUniform_ = vertexShaderUniform;
    }
    const VertexShaderUniform& getVertexShaderUniform() const {
        return vertexShaderUniform_;
    }
    VertexShaderUniform& getVertexShaderUniform() {
        return vertexShaderUniform_;
    }

    void setFragmentShaderUniform(const FragmentShaderUniform& fragmentShaderUniform) {
        fragmentShaderUniform_ = fragmentShaderUniform;
    }
    const FragmentShaderUniform& getFragmentShaderUniform() const {
        return fragmentShaderUniform_;
    }
    FragmentShaderUniform& getFragmentShaderUniform() {
        return fragmentShaderUniform_;
    }

    void setTransform(const ObjectTransform& objectTransform) {
        objectTransform_ = objectTransform;
    }
    const ObjectTransform& getTransform() const {
        return objectTransform_;
    }
    ObjectTransform& getTransform() {
        return objectTransform_;
    }

    void setMeshData(MeshDataRef mesh) {
        mesh_ = mesh;
    }
    MeshDataRef getMeshData() const {
        return mesh_;
    }

    void setDrawingMode(bool wireframeMode = false, bool drawingEnabled = true, bool writeToDepthBuffer = true, glm::vec3 wireframeColor = glm::vec3(1.0f)) {
        wireframeMode_ = wireframeMode;
        drawingEnabled_ = drawingEnabled;
        writeToDepthBuffer_ = writeToDepthBuffer;
        wireframeColor_ = wireframeColor;
    }

    bool getWireframeMode() const {
        return wireframeMode_;
    }

    bool getWriteToDepthBuffer() const {
        return writeToDepthBuffer_;
    }

    bool getDrawingEnabled() const {
        return drawingEnabled_;
    }

    glm::vec3 getWireframeColor() const {
        return wireframeColor_;
    }

    size_t getTriangleCount() const {
        return mesh_.get().faces.size();
    }

 private:
    MeshDataRef mesh_;
    VertexShaderUniform vertexShaderUniform_;
    FragmentShaderUniform fragmentShaderUniform_;
    ObjectTransform objectTransform_;

    glm::vec3 wireframeColor_ = glm::vec3(1.0f);
    bool wireframeMode_ = false;
    bool drawingEnabled_ = true;
    bool writeToDepthBuffer_ = true;

    VertexShader vertexShader_;
    FragmentShader fragmentShader_;
};

}  // namespace eng
