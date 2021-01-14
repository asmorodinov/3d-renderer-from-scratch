#include "Object.h"

namespace eng {

Transform& Object::getTransform() { return t; }

glm::vec3 Object::getPosition() const { return t.position; }
void Object::setPosition(glm::vec3 pos) { t.position = pos; }

MeshData loadFromObj(const std::string& filename, float scale, bool invertNormals, bool uvCoords) {
    std::ifstream file(filename);
    assert(file);

    MeshData mesh;
    if (uvCoords == false) {
        mesh.textureCoords.push_back(glm::vec2(0.5f));
    }

    std::string s;
    while (file >> s) {
        if (s == "v") {
            float x, y, z;
            file >> x >> y >> z;

            mesh.vertices.push_back(scale * glm::vec3(x, y, z));
        } else if (s == "vt") {
            float u, v;
            file >> u >> v;
            mesh.textureCoords.push_back({u, v});
        } else if (s == "f") {
            if (uvCoords == false) {
                int i, j, k;
                file >> i >> j >> k;

                if (invertNormals) std::swap(i, j);
                mesh.faces.push_back({i - 1, j - 1, k - 1, 0, 0, 0});
            } else {
                int i, j, k;
                int ti, tj, tk;
                int ni, nj, nk;
                char c;
                file >> i >> c >> ti >> c >> ni >> j >> c >> tj >> c >> nj >> k >> c >> tk >> c >> nk;

                if (invertNormals) {
                    std::swap(i, j);
                    std::swap(ti, tj);
                }
                mesh.faces.push_back({i - 1, j - 1, k - 1, ti - 1, tj - 1, tk - 1});
            }
        } else {
            std::string line;
            std::getline(file, line);
        }
    }

    return mesh;
}

MeshData MeshData::generateCubeData(float sz, bool invertNormals) {
    auto res = MeshData{{{-sz, -sz, -sz},
                         {-sz, -sz, sz},
                         {-sz, sz, -sz},
                         {-sz, sz, sz},
                         {sz, -sz, -sz},
                         {sz, -sz, sz},
                         {sz, sz, -sz},
                         {sz, sz, sz}},
                        {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                        {{1, 3, 7, 0, 1, 2},
                         {1, 7, 5, 0, 2, 3},
                         {5, 7, 6, 0, 1, 2},
                         {5, 6, 4, 0, 2, 3},
                         {4, 6, 2, 0, 1, 2},
                         {4, 2, 0, 0, 2, 3},
                         {0, 2, 3, 0, 1, 2},
                         {0, 3, 1, 0, 2, 3},
                         {3, 2, 6, 0, 1, 2},
                         {3, 6, 7, 0, 2, 3},
                         {0, 1, 5, 0, 1, 2},
                         {0, 5, 4, 0, 2, 3}}};

    if (invertNormals)
        for (auto& face : res.faces) std::swap(face.i, face.j);

    return res;
}

Mesh::Mesh(const MeshData& mesh, Texture* texture, ColorType color, Shader& ph, Shader& fl, Shader& td, Shader& uv, Shader& nrm,
           std::optional<RenderMode> rm)
    : mesh(mesh), texture(texture), color(color), ph(ph), fl(fl), td(td), uv(uv), nrm(nrm), rm(rm) {}

void Mesh::draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) {
    glm::mat4 model = t.getModel();
    glm::mat4 transform = camera.getViewMatrix();
    glm::mat4 projection = screen.getProjectionMatrix();
    float near = screen.near;

    if (rm.has_value()) r = *rm;

    for (auto& face : mesh.faces) {
        glm::vec4 v0 = glm::vec4(mesh.vertices[face.i], 1.0f);
        glm::vec4 v1 = glm::vec4(mesh.vertices[face.j], 1.0f);
        glm::vec4 v2 = glm::vec4(mesh.vertices[face.k], 1.0f);

        glm::vec4 p0_ = model * v0;
        glm::vec4 p1_ = model * v1;
        glm::vec4 p2_ = model * v2;
        p0_ /= p0_.w;
        p1_ /= p1_.w;
        p2_ /= p2_.w;
        glm::vec3 p0 = glm::vec3(p0_);
        glm::vec3 p1 = glm::vec3(p1_);
        glm::vec3 p2 = glm::vec3(p2_);
        glm::vec3 normal = -glm::normalize(glm::cross(p1 - p0, p2 - p0));

        Triangle tr;
        p0_ = transform * model * v0;
        p1_ = transform * model * v1;
        p2_ = transform * model * v2;
        tr.p0 = glm::vec4(p0_.x / p0_.w, p0_.y / p0_.w, p0_.z / p0_.w, p0_.w);
        tr.p1 = glm::vec4(p1_.x / p1_.w, p1_.y / p1_.w, p1_.z / p1_.w, p1_.w);
        tr.p2 = glm::vec4(p2_.x / p2_.w, p2_.y / p2_.w, p2_.z / p2_.w, p2_.w);

        if (r == RenderMode::Texture || r == RenderMode::UV) {
            tr.v0 = ShaderVariablesVec({mesh.textureCoords[face.ti]});
            tr.v1 = ShaderVariablesVec({mesh.textureCoords[face.tj]});
            tr.v2 = ShaderVariablesVec({mesh.textureCoords[face.tk]});
            if (r == RenderMode::Texture) {
                td.setConst({texture});
                drawTriangle(tr, projection, near, td, screen, lights);
            } else {
                drawTriangle(tr, projection, near, uv, screen, lights);
            }
        } else if (r == RenderMode::FlatColor) {
            fl.setConst({color});

            drawTriangle(tr, projection, near, fl, screen, lights);
        } else if (r == RenderMode::Wireframe) {
            drawWireframeTriangle(tr, projection, near, color, screen);
        } else {
            if (r == RenderMode::Phong) {
                tr.v0 = ShaderVariablesVec({p0, mesh.textureCoords[face.ti]});
                tr.v1 = ShaderVariablesVec({p1, mesh.textureCoords[face.tj]});
                tr.v2 = ShaderVariablesVec({p2, mesh.textureCoords[face.tk]});

                ph.setConst({texture, camera.getPosition(), normal});

                drawTriangle(tr, projection, near, ph, screen, lights);
            } else {
                nrm.setConst({normal});

                drawTriangle(tr, projection, near, nrm, screen, lights);
            }
        }
    }
}

void Mesh::update(float dt) {}

size_t Mesh::getTriangleCount() { return mesh.faces.size(); }

ColorType Mesh::getColor() const { return color; }
void Mesh::setColor(ColorType clr) { color = clr; }

Skybox::Skybox(const MeshData& mesh, Shader& shader, std::optional<RenderMode> rm) : mesh(mesh), shader(shader), rm(rm) {}

void Skybox::draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) {
    // remove translation
    glm::mat4 model = glm::mat4(glm::mat3(t.getModel()));

    glm::mat4 transform = glm::mat4(glm::mat3(camera.getViewMatrix()));
    glm::mat4 projection = screen.getProjectionMatrix();
    float near = screen.near;

    if (rm.has_value()) r = *rm;

    for (auto& face : mesh.faces) {
        glm::vec4 v0 = glm::vec4(mesh.vertices[face.i], 1.0f);
        glm::vec4 v1 = glm::vec4(mesh.vertices[face.j], 1.0f);
        glm::vec4 v2 = glm::vec4(mesh.vertices[face.k], 1.0f);

        glm::vec4 p0_ = model * v0;
        glm::vec4 p1_ = model * v1;
        glm::vec4 p2_ = model * v2;
        p0_ /= p0_.w;
        p1_ /= p1_.w;
        p2_ /= p2_.w;
        glm::vec3 p0 = glm::vec3(p0_);
        glm::vec3 p1 = glm::vec3(p1_);
        glm::vec3 p2 = glm::vec3(p2_);

        Triangle tr;
        p0_ = transform * model * v0;
        p1_ = transform * model * v1;
        p2_ = transform * model * v2;
        tr.p0 = glm::vec4(p0_.x / p0_.w, p0_.y / p0_.w, p0_.z / p0_.w, p0_.w);
        tr.p1 = glm::vec4(p1_.x / p1_.w, p1_.y / p1_.w, p1_.z / p1_.w, p1_.w);
        tr.p2 = glm::vec4(p2_.x / p2_.w, p2_.y / p2_.w, p2_.z / p2_.w, p2_.w);

        if (r != RenderMode::Wireframe) {
            tr.v0 = ShaderVariablesVec({p0});
            tr.v1 = ShaderVariablesVec({p1});
            tr.v2 = ShaderVariablesVec({p2});

            screen.setDepth = false;

            drawTriangle(tr, projection, near, shader, screen, lights);

            screen.setDepth = true;
        } else {
            drawWireframeTriangle(tr, projection, near, glm::vec3(1.0f), screen);
        }
    }
}

void Skybox::update(float dt) {}

size_t Skybox::getTriangleCount() { return mesh.faces.size(); }

}  // namespace eng
