#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace eng {

class Transform {
 public:
    Transform() = default;
    Transform(glm::vec3 pos);

    glm::mat4 getModel();

    glm::vec3 getPosition() const;
    glm::quat getOrientatio() const;
    glm::vec3 getScale() const;

    void setPosition(const glm::vec3 pos);
    void setOrientation(const glm::quat orient);
    void setScale(const glm::vec3 sc);

 private:
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat orientation = glm::quat();
    glm::vec3 scale = glm::vec3(1.0f);

    bool modelInvalid = true;
    glm::mat4 model = glm::mat4(1.0f);
};

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

    static MeshData generateCubeData(float sz = 1.0f, bool invertNormals = false);
};

MeshData loadFromObj(const std::string& filename, float scale = 1.0f, bool invertNormals = false, bool onlyVertices = false);

}  // namespace eng
