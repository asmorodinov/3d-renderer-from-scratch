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

#include "Common.h"
#include "VertexShader.h"

namespace eng {

template <typename VertexShaderUniform, typename FragmentShaderUniform, typename VertexShaderOutput, typename VertexShader,
          typename FragmentShader>
class Mesh {
 public:
    using VertexShaderRef = std::reference_wrapper<VertexShader>;
    using FragmentShaderRef = std::reference_wrapper<FragmentShader>;
    using MeshDataRef = std::reference_wrapper<const MeshData>;

    Mesh(MeshDataRef mesh_, const VertexShaderUniform& vu_, const FragmentShaderUniform& fu_, const Transform& t_,
         VertexShaderRef vshader_, FragmentShaderRef fshader_, bool wf = false, bool write = true)
        : mesh(mesh_),
          vu(vu_),
          fu(fu_),
          t(t_),
          vshader(vshader_),
          fshader(fshader_),
          wireframeMode(wf),
          writeToDepthBuffer(write) {}

    void draw(const Camera& camera, Screen& screen, const LightsVec& lights) const {
        if (!drawingEnabled) return;

        glm::mat4 model = t.getModel();
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = screen.getProjectionMatrix();
        glm::vec3 viewPos = camera.getPosition();

        auto& vs = vshader.get();
        auto& fs = fshader.get();

        vs.uniform = vu;
        fs.uniform = fu;

        vs.setMVP(model, view, projection, viewPos);

        const auto& m = mesh.get();

        for (const auto& face : m.faces) {
            VertexShaderOutput out = vs.run({m.vertices[face.i], m.vertices[face.j], m.vertices[face.k], m.textureCoords[face.ti],
                                             m.textureCoords[face.tj], m.textureCoords[face.tk], m.normals[face.ni],
                                             m.normals[face.nj], m.normals[face.nk]});
            auto tr = out.triangle;
            fs.vso = out.uniformOutput;

            if (wireframeMode) {
                drawWireframeTriangle({tr.p0, tr.p1, tr.p2, {}, {}, {}}, projection, wireframeColor, screen);
            } else {
                if (!writeToDepthBuffer) screen.setWriteToDepthBuffer(false);

                drawTriangle(tr, projection, fs, screen, lights);

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

    VertexShaderRef vshader;
    FragmentShaderRef fshader;
};

using FlatMesh = Mesh<FlatVertexShader::Uniform, FlatShader::Uniform, FlatVertexShader::Output, FlatVertexShader, FlatShader>;
using TextureMesh =
    Mesh<TextureVertexShader::Uniform, TextureShader::Uniform, TextureVertexShader::Output, TextureVertexShader, TextureShader>;

using CubemapMesh =
    Mesh<CubemapVertexShader::Uniform, CubemapShader::Uniform, CubemapVertexShader::Output, CubemapVertexShader, CubemapShader>;

using UVMesh = Mesh<UVVertexShader::Uniform, UVShader::Uniform, UVVertexShader::Output, UVVertexShader, UVShader>;
using NormalMesh =
    Mesh<NormalVertexShader::Uniform, NormalShader::Uniform, NormalVertexShader::Output, NormalVertexShader, NormalShader>;

using PhongMesh =
    Mesh<PhongVertexShader::Uniform, PhongShader::Uniform, PhongVertexShader::Output, PhongVertexShader, PhongShader>;

using NormalMapMesh = Mesh<NormalMapVertexShader::Uniform, NormalMapShader::Uniform, NormalMapVertexShader::Output,
                           NormalMapVertexShader, NormalMapShader>;

struct ObjectsVec {
    std::vector<FlatMesh> flatMeshes;
    std::vector<TextureMesh> textureMeshes;
    std::vector<CubemapMesh> cubemapMeshes;
    std::vector<UVMesh> uvMeshes;
    std::vector<NormalMesh> normalMeshes;
    std::vector<PhongMesh> phongMeshes;
    std::vector<NormalMapMesh> normalMapMeshes;
};

}  // namespace eng
