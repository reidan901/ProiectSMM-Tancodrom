#pragma once
#include "Mesh.h"
class Model
{
public:
    Model(std::vector<Mesh> mesh)
    {
        this->meshes = mesh;
    }
    void Draw(Shader& shader);
private:
    // model data
    std::vector<Mesh> meshes;
};