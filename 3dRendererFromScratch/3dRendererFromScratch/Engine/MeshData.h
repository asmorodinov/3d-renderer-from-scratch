#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eng {

struct Face {
    // vertex indices
    int i, j, k;
    // texture indices
    int ti, tj, tk;
    // normal indices
    int ni, nj, nk;
};

struct MeshData {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> textureCoords;
    std::vector<Face> faces;

    static MeshData generateCubeData(float size = 1.0f, bool invertNormals = false);

    std::string fileName;
    float scale = 1.0f;
    bool invertNormals = false;
    bool onlyVertices = false;
};

MeshData loadFromObj(const std::string& filename, float scale = 1.0f, bool invertNormals = false, bool onlyVertices = false);

}  // namespace eng
