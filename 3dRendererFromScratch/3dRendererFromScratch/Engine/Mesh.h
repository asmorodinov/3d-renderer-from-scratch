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

template <typename VertexShaderUniform, typename FragmentShaderUniform, typename VertexShaderOutput, typename VertexShader, typename FragmentShader>
class Mesh {
 public:
    using MeshDataRef = std::reference_wrapper<const MeshData>;

    Mesh(MeshDataRef mesh_, const VertexShaderUniform& vu_, const FragmentShaderUniform& fu_, const ObjectTransform& t_, bool wf = false, bool write = true,
         glm::vec3 wfc = glm::vec3(1.0f))
        : mesh_(mesh_),
          vertexShaderUniform_(vu_),
          fragmentShaderUniform_(fu_),
          objectTransform_(t_),
          vertexShader_(),
          fragmentShader_(),
          wireframeMode_(wf),
          writeToDepthBuffer_(write),
          wireframeColor_(wfc) {}

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
        if (!drawingEnabled_) return;

        glm::mat4 model = objectTransform_.getModel();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = screen.getProjectionMatrix();
        glm::vec3 viewPos = camera.getPosition();

        vertexShader_.uniform = vertexShaderUniform_;
        fragmentShader_.uniform = fragmentShaderUniform_;

        vertexShader_.setMVP(model, view, projection, viewPos);

        const auto& m = mesh_.get();

        for (const auto& face : m.faces) {
            VertexShaderOutput out =
                vertexShader_.run({m.vertices[face.i], m.vertices[face.j], m.vertices[face.k], m.textureCoords[face.ti], m.textureCoords[face.tj],
                                   m.textureCoords[face.tk], m.normals[face.ni], m.normals[face.nj], m.normals[face.nk]});
            auto tr = out.triangle;
            fragmentShader_.vso = out.uniformOutput;

            if (wireframeMode_) {
                drawWireframeTriangle({tr.p0, tr.p1, tr.p2, {}, {}, {}}, projection, wireframeColor_, screen);
            } else {
                if (!writeToDepthBuffer_) screen.setWriteToDepthBuffer(false);

                drawTriangle(tr, projection, fragmentShader_, screen, lights);

                if (!writeToDepthBuffer_) screen.setWriteToDepthBuffer(true);
            }
        }
    }

    void setVertexShaderUniform(const VertexShaderUniform& vu_) { vertexShaderUniform_ = vu_; }
    const VertexShaderUniform& getVertexShaderUniform() const { return vertexShaderUniform_; }
    VertexShaderUniform& getVertexShaderUniform() { return vertexShaderUniform_; }

    void setFragmentShaderUniform(const FragmentShaderUniform& fu_) { fragmentShaderUniform_ = fu_; }
    const FragmentShaderUniform& getFragmentShaderUniform() const { return fragmentShaderUniform_; }
    FragmentShaderUniform& getFragmentShaderUniform() { return fragmentShaderUniform_; }

    void setTransform(const ObjectTransform& t_) { objectTransform_ = t_; }
    const ObjectTransform& getTransform() const { return objectTransform_; }
    ObjectTransform& getTransform() { return objectTransform_; }

    void setDrawingMode(bool wf = false, bool en = true, bool write = true, glm::vec3 wfc = glm::vec3(1.0f)) {
        wireframeMode_ = wf;
        drawingEnabled_ = en;
        writeToDepthBuffer_ = write;
        wireframeColor_ = wfc;
    }

    size_t getTriangleCount() const { return mesh_.get().faces.size(); }

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
