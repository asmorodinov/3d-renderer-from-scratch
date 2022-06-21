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
         bool drawingEnabled = true, bool transparent = false, bool drawBackface = false)
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
          isTransparent_(transparent),
          drawBackface_(drawBackface) {
    }

    glm::vec3 getCenter(const eng::MeshData& mesh, const eng::Face& face, const glm::mat4& model) {
        auto p0 = glm::vec3(model * glm::vec4(mesh.vertices[face.i], 1.0f));
        auto p1 = glm::vec3(model * glm::vec4(mesh.vertices[face.j], 1.0f));
        auto p2 = glm::vec3(model * glm::vec4(mesh.vertices[face.k], 1.0f));
        auto center = (p0 + p1 + p2) / 3.0f;
        return center;
    }

    std::vector<Distance> getDistances(const Camera& camera) {
        auto res = std::vector<Distance>();

        const auto& mesh = mesh_.get();
        const auto& viewPos = camera.getPosition();

        const auto& model = objectTransform_.getModel();

        for (const auto& face : mesh.faces) {
            auto center = getCenter(mesh, face, model);
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

        auto i = face.i;
        auto j = face.j;
        auto ti = face.ti;
        auto tj = face.tj;

        if (drawBackface_) {
            auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            auto normal = glm::normalize(normalMatrix * mesh.normals[face.ni]);
            auto center = getCenter(mesh, face, model);
            auto viewDir = glm::normalize(viewPos - center);

            if (glm::dot(viewDir, normal) < 0.0f) {
                std::swap(i, j);
                std::swap(ti, tj);
            }
        }

        VertexShaderOutput out = vertexShader_.run({
            mesh.vertices[i], mesh.vertices[j], mesh.vertices[face.k],                    // vertices
            mesh.textureCoords[ti], mesh.textureCoords[tj], mesh.textureCoords[face.tk],  // texture coords
            mesh.tangents[face.ni],                                                       // tangent
            mesh.bitangents[face.ni],                                                     // bitangent
            mesh.normals[face.ni]                                                         // normal
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

    bool getDrawBackface() const {
        return drawBackface_;
    }

    // not needed in most cases, but useful for things like deferred rendering pipeline, where we are using special shaders with additional uniform variables
    FragmentShader& getFragmentShader() {
        return fragmentShader_;
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
    bool drawBackface_ = false;

    VertexShader vertexShader_;
    FragmentShader fragmentShader_;
};

template <typename VS1, typename FS1, typename VS2, typename FS2>
void copyMeshParamsToOtherMesh(Mesh<VS1, FS1>& to, const Mesh<VS2, FS2>& from) {
    to.setMeshData(from.getMeshData());
    to.setTransform(from.getTransform());
    to.setDrawingMode(false, from.getDrawingEnabled(), from.getWriteToDepthBuffer());
    to.setFragmentShaderUniform(from.getFragmentShaderUniform());
}

}  // namespace eng
