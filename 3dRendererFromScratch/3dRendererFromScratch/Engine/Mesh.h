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

#include "Common.h"
namespace eng {

template <typename VertexShaderUniform, typename FragmentShaderUniform, typename VertexShaderOutput, typename VertexShader,
          typename FragmentShader>
class Mesh {
 public:
    using MeshDataRef = std::reference_wrapper<const MeshData>;

    Mesh(MeshDataRef mesh_, const VertexShaderUniform& vu_, const FragmentShaderUniform& fu_, const Transform& t_,
         bool wf = false, bool write = true)
        : mesh(mesh_), vu(vu_), fu(fu_), t(t_), vshader(), fshader(), wireframeMode(wf), writeToDepthBuffer(write) {}

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) {
        if (!drawingEnabled) return;

        glm::mat4 model = t.getModel();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = screen.getProjectionMatrix();
        glm::vec3 viewPos = camera.getPosition();

        vshader.uniform = vu;
        fshader.uniform = fu;

        vshader.setMVP(model, view, projection, viewPos);

        const auto& m = mesh.get();

        for (const auto& face : m.faces) {
            VertexShaderOutput out = vshader.run({m.vertices[face.i], m.vertices[face.j], m.vertices[face.k],
                                                  m.textureCoords[face.ti], m.textureCoords[face.tj], m.textureCoords[face.tk],
                                                  m.normals[face.ni], m.normals[face.nj], m.normals[face.nk]});
            auto tr = out.triangle;
            fshader.vso = out.uniformOutput;

            if (wireframeMode) {
                drawWireframeTriangle({tr.p0, tr.p1, tr.p2, {}, {}, {}}, projection, wireframeColor, screen);
            } else {
                if (!writeToDepthBuffer) screen.setWriteToDepthBuffer(false);

                drawTriangle(tr, projection, fshader, screen, lights);

                if (!writeToDepthBuffer) screen.setWriteToDepthBuffer(true);
            }
        }
    }

    void setVertexShaderUniform(const VertexShaderUniform& vu_) { vu = vu_; }
    const VertexShaderUniform& getVertexShaderUniform() const { return vu; }
    VertexShaderUniform& getVertexShaderUniform() { return vu; }

    void setFragmentShaderUniform(const FragmentShaderUniform& fu_) { fu = fu_; }
    const FragmentShaderUniform& getFragmentShaderUniform() const { return fu; }
    FragmentShaderUniform& getFragmentShaderUniform() { return fu; }

    void setTransform(const Transform& t_) { t = t_; }
    const Transform& getTransform() const { return t; }
    Transform& getTransform() { return t; }

    bool setDrawingMode(bool wf = false, bool en = true, bool write = true) {
        wireframeMode = wf;
        drawingEnabled = en;
        writeToDepthBuffer = write;
    }

    size_t getTriangleCount() const { return mesh.get().faces.size(); }

 private:
    MeshDataRef mesh;
    VertexShaderUniform vu;
    FragmentShaderUniform fu;
    Transform t;

    glm::vec3 wireframeColor = glm::vec3(1.0f);
    bool wireframeMode = false;
    bool drawingEnabled = true;
    bool writeToDepthBuffer = true;

    VertexShader vshader;
    FragmentShader fshader;
};

}  // namespace eng
