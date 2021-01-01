#include "Object.h"

namespace eng {

Transform& Object::getTransform() { return t; }

MeshData loadFromObj(const std::string& filename, float scale, bool invertNormals, bool uvCoords) {
    std::ifstream file(filename);
    assert(file);

    MeshData mesh;
    if (uvCoords == false) {
        mesh.textureCoords.push_back(glm::vec2());
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

Mesh::Mesh(const MeshData& mesh, Texture* texture, ColorType color, Shader& ph, Shader& fl, Shader& td,
           std::optional<RenderMode> rm)
    : mesh(mesh), texture(texture), color(color), ph(ph), fl(fl), td(td), rm(rm) {}

void Mesh::draw(RenderMode r, const Camera& camera, Screen& screen, const LightsVec& lights) {
    glm::mat4 model = t.getModel();
    glm::mat4 transform = screen.getProjectionMatrix() * camera.getViewMatrix();

    if (rm.has_value()) r = *rm;

    for (auto& face : mesh.faces) {
        glm::vec4 p0_ = model * glm::vec4(mesh.vertices[face.i], 1.0f);
        glm::vec4 p1_ = model * glm::vec4(mesh.vertices[face.j], 1.0f);
        glm::vec4 p2_ = model * glm::vec4(mesh.vertices[face.k], 1.0f);
        p0_ /= p0_.w;
        p1_ /= p1_.w;
        p2_ /= p2_.w;
        glm::vec3 p0 = glm::vec3(p0_);
        glm::vec3 p1 = glm::vec3(p1_);
        glm::vec3 p2 = glm::vec3(p2_);

        Triangle tr;
        tr.p0 = p0;
        tr.p1 = p1;
        tr.p2 = p2;

        if (r == RenderMode::Texture) {
            tr.v0 = ShaderVariablesVec({mesh.textureCoords[face.ti]});
            tr.v1 = ShaderVariablesVec({mesh.textureCoords[face.tj]});
            tr.v2 = ShaderVariablesVec({mesh.textureCoords[face.tk]});

            td.setConst({texture});
            drawTriangle(tr, transform, td, screen, lights);
        } else if (r == RenderMode::FlatColor) {
            fl.setConst({color});

            drawTriangle(tr, transform, fl, screen, lights);
        } else if (r == RenderMode::Wireframe) {
            drawLine(p0, p1, color, transform, screen);
            drawLine(p1, p2, color, transform, screen);
            drawLine(p2, p0, color, transform, screen);
        } else {
            tr.v0 = ShaderVariablesVec({p0, mesh.textureCoords[face.ti]});
            tr.v1 = ShaderVariablesVec({p1, mesh.textureCoords[face.tj]});
            tr.v2 = ShaderVariablesVec({p2, mesh.textureCoords[face.tk]});

            glm::vec3 normal = -glm::normalize(glm::cross(p1 - p0, p2 - p0));

            ph.setConst({texture, camera.getPosition(), normal});

            drawTriangle(tr, transform, ph, screen, lights);
        }
    }
}

void Mesh::update(float dt) {}

size_t Mesh::getTriangleCount() { return mesh.faces.size(); }

ColorType Mesh::getColor() const { return color; }
void Mesh::setColor(ColorType clr) { color = clr; }

glm::vec3 Mesh::getPosition() const { return t.position; }
void Mesh::setPosition(glm::vec3 pos) { t.position = pos; }

}  // namespace eng
