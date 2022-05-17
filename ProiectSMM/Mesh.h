#pragma once
#include <GLM.hpp>
#include <string>
#include "Shader.h"
#include <vector>
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct Material {
    std::string name;
    float shininess;
    int diffusetextureID;
    int speculartextureID;
};
class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    Material MeshMaterial;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
    Mesh() = default;
    void Draw(Shader& shader);
private:
    //  render data
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

