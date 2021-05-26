#include "MeshData.h"

namespace eng {

MeshData MeshData::generateCubeData(float size, bool invertNormals) {
    auto res = MeshData{{{-size, -size, -size},
                         {-size, -size, size},
                         {-size, size, -size},
                         {-size, size, size},
                         {size, -size, -size},
                         {size, -size, size},
                         {size, size, -size},
                         {size, size, size}},
                        {{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}},
                        {{0, 0}, {0, 1}, {1, 1}, {1, 0}},
                        {{1, 7, 3, 0, 2, 1, 0, 0, 0},
                         {1, 5, 7, 0, 3, 2, 0, 0, 0},
                         {5, 6, 7, 0, 2, 1, 4, 4, 4},
                         {5, 4, 6, 0, 3, 2, 4, 4, 4},
                         {4, 2, 6, 0, 2, 1, 1, 1, 1},
                         {4, 0, 2, 0, 3, 2, 1, 1, 1},
                         {0, 3, 2, 0, 2, 1, 5, 5, 5},
                         {0, 1, 3, 0, 3, 2, 5, 5, 5},
                         {3, 6, 2, 0, 2, 1, 2, 2, 2},
                         {3, 7, 6, 0, 3, 2, 2, 2, 2},
                         {0, 5, 1, 0, 2, 1, 3, 3, 3},
                         {0, 4, 5, 0, 3, 2, 3, 3, 3}}};

    if (invertNormals) {
        for (auto& face : res.faces) {
            std::swap(face.i, face.j);
            std::swap(face.ti, face.tj);
            std::swap(face.ni, face.nj);
        }
        for (auto& normal : res.normals) normal *= -1.0f;
    }
    return res;
}

MeshData loadFromObj(const std::string& filename, float scale, bool invertNormals, bool onlyVertices) {
    std::ifstream file(filename);
    assert(file);

    MeshData mesh;
    if (onlyVertices) {
        mesh.textureCoords.push_back(glm::vec2(0.5f));
    }

    unsigned faceCnt = 0;

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
        } else if (s == "vn") {
            float x, y, z;
            file >> x >> y >> z;
            if (invertNormals) {
                x *= -1.0f;
                y *= -1.0f;
                z *= -1.0f;
            }
            mesh.normals.push_back({x, y, z});
        } else if (s == "f") {
            int i, j, k;
            int ti = 1, tj = 1, tk = 1;
            int ni = faceCnt + 1, nj = faceCnt + 1, nk = faceCnt + 1;
            ++faceCnt;

            if (onlyVertices) {
                file >> i >> j >> k;

                if (invertNormals) std::swap(i, j);
            } else {
                char c;
                file >> i >> c >> ti >> c >> ni >> j >> c >> tj >> c >> nj >> k >> c >> tk >> c >> nk;

                if (invertNormals) {
                    std::swap(i, j);
                    std::swap(ti, tj);
                    std::swap(ni, nj);
                }
            }

            if (onlyVertices) {
                glm::vec3 v0 = mesh.vertices[i - 1];
                glm::vec3 v1 = mesh.vertices[j - 1];
                glm::vec3 v2 = mesh.vertices[k - 1];
                glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
                mesh.normals.push_back(normal);
            }

            mesh.faces.push_back({i - 1, j - 1, k - 1, ti - 1, tj - 1, tk - 1, ni - 1, nj - 1, nk - 1});
        } else {
            std::string line;
            std::getline(file, line);
        }
    }
    file.close();

    return mesh;
}

}  // namespace eng
