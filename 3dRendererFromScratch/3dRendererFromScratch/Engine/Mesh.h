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
#include "ProjectionInfo.h"
#include "Light.h"

#include "MeshData.h"
#include "ObjectTransform.h"

#include "Types.h"

#include "Shaders/BasicShader.h"

namespace eng {

template <typename VertexShader, typename FragmentShader>
class Mesh {
 public:
    using MeshDataRef = std::reference_wrapper<const MeshData>;
    using VertexShaderUniform = VertexShaderUniform;
    using FragmentShaderUniform = FragmentShaderUniform;
    using VertexShaderOutput = typename VertexShader::Output;

    Mesh(MeshDataRef mesh, const VertexShaderUniform& vertexShaderUniform, const FragmentShaderUniform& fragmentShaderUniform,
         const ObjectTransform& objectTransform, bool wireframeMode = false, bool writeToDepthBuffer = true, Color32 wireframeColor = Color32(255),
         bool drawingEnabled = true, bool transparent = false)
        : mesh_(mesh),
          vertexShaderUniform_(vertexShaderUniform),
          fragmentShaderUniform_(fragmentShaderUniform),
          objectTransform_(objectTransform),
          vertexShader_(),
          fragmentShader_(),
          wireframeMode_(wireframeMode),
          writeToDepthBuffer_(writeToDepthBuffer),
          drawingEnabled_(drawingEnabled),
          wireframeColor_(wireframeColor),
          isTransparent_(transparent) {
    }

    std::vector<Distance> getDistances(const Camera& camera) {
        auto res = std::vector<Distance>();

        const auto& mesh = mesh_.get();
        const auto& viewPos = camera.getPosition();

        const auto& model = objectTransform_.getModel();

        for (const auto& face : mesh.faces) {
            auto p0 = glm::vec3(model * glm::vec4(mesh.vertices[face.i], 1.0f));
            auto p1 = glm::vec3(model * glm::vec4(mesh.vertices[face.j], 1.0f));
            auto p2 = glm::vec3(model * glm::vec4(mesh.vertices[face.k], 1.0f));
            auto center = (p0 + p1 + p2) / 3.0f;
            res.push_back(glm::length(center - viewPos));
        }

        return res;
    }

    template <typename Buffer>
    void draw(const Camera& camera, ProjectionInfo& projectionInfo, const LightsVec& lights, Buffer& buffer) {
        if (!drawingEnabled_) return;

        const auto& model = objectTransform_.getModel();
        const auto& view = camera.getViewMatrix();
        const auto& projection = projectionInfo.getProjectionMatrix();
        const auto& viewPos = camera.getPosition();

        vertexShader_.uniform = vertexShaderUniform_;
        fragmentShader_.uniform = fragmentShaderUniform_;

        vertexShader_.setMVP(model, view, projection, viewPos);

        const auto& mesh = mesh_.get();

        for (const auto& face : mesh.faces) {
            VertexShaderOutput out = vertexShader_.run({
                mesh.vertices[face.i], mesh.vertices[face.j], mesh.vertices[face.k],                    // vertices
                mesh.textureCoords[face.ti], mesh.textureCoords[face.tj], mesh.textureCoords[face.tk],  // texture coords
                mesh.tangents[face.ni],                                                                 // tangent
                mesh.bitangents[face.ni],                                                               // bitangent
                mesh.normals[face.ni]                                                                   // normal
            });

            const auto& triangle = out.triangle;
            fragmentShader_.vso = out.uniformOutput;

            if (wireframeMode_) {
                drawWireframeTriangle({triangle.p0, triangle.p1, triangle.p2, {}, {}, {}}, projection, wireframeColor_, projectionInfo, buffer);
            } else {
                if (!writeToDepthBuffer_) buffer.setWriteToDepthBuffer(false);

                drawTriangle(triangle, projection, fragmentShader_, projectionInfo, lights, buffer);

                if (!writeToDepthBuffer_) buffer.setWriteToDepthBuffer(true);
            }
        }
    }

    template <typename Buffer>
    void drawFace(const Camera& camera, ProjectionInfo& projectionInfo, const LightsVec& lights, Buffer& buffer, size_t faceIndex) {
        if (!drawingEnabled_) return;

        const auto& model = objectTransform_.getModel();
        const auto& view = camera.getViewMatrix();
        const auto& projection = projectionInfo.getProjectionMatrix();
        const auto& viewPos = camera.getPosition();

        vertexShader_.uniform = vertexShaderUniform_;
        fragmentShader_.uniform = fragmentShaderUniform_;

        vertexShader_.setMVP(model, view, projection, viewPos);

        const auto& mesh = mesh_.get();

        const auto& face = mesh.faces[faceIndex];
        VertexShaderOutput out = vertexShader_.run({
            mesh.vertices[face.i], mesh.vertices[face.j], mesh.vertices[face.k],                    // vertices
            mesh.textureCoords[face.ti], mesh.textureCoords[face.tj], mesh.textureCoords[face.tk],  // texture coords
            mesh.tangents[face.ni],                                                                 // tangent
            mesh.bitangents[face.ni],                                                               // bitangent
            mesh.normals[face.ni]                                                                   // normal
        });

        const auto& triangle = out.triangle;
        fragmentShader_.vso = out.uniformOutput;

        if (wireframeMode_) {
            drawWireframeTriangle({triangle.p0, triangle.p1, triangle.p2, {}, {}, {}}, projection, wireframeColor_, projectionInfo, buffer);
        } else {
            if (!writeToDepthBuffer_) buffer.setWriteToDepthBuffer(false);

            drawTriangle(triangle, projection, fragmentShader_, projectionInfo, lights, buffer);

            if (!writeToDepthBuffer_) buffer.setWriteToDepthBuffer(true);
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

    void setDrawingMode(bool wireframeMode = false, bool drawingEnabled = true, bool writeToDepthBuffer = true, Color32 wireframeColor = Color32(255),
                        bool transparent = false) {
        wireframeMode_ = wireframeMode;
        drawingEnabled_ = drawingEnabled;
        writeToDepthBuffer_ = writeToDepthBuffer;
        wireframeColor_ = wireframeColor;
        isTransparent_ = transparent;
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
    void setWireframeColor(Color32 color) {
        wireframeColor_ = color;
    }

    bool getIsTransparent() const {
        return isTransparent_;
    }
    void setIsTransparent(bool transparent) {
        isTransparent_ = transparent;
    }

    size_t getTriangleCount() const {
        return mesh_.get().faces.size();
    }

 private:
    MeshDataRef mesh_;
    VertexShaderUniform vertexShaderUniform_;
    FragmentShaderUniform fragmentShaderUniform_;
    ObjectTransform objectTransform_;

    Color32 wireframeColor_ = Color32(255);
    bool wireframeMode_ = false;
    bool drawingEnabled_ = true;
    bool writeToDepthBuffer_ = true;
    bool isTransparent_ = false;

    VertexShader vertexShader_;
    FragmentShader fragmentShader_;
};

}  // namespace eng
