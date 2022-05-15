
#include "Loader.h"
#include <OBJ_Loader.h>
#include "TextureUtilities.h"
#include <map>

std::vector<Mesh> Loader::LoadObj(std::string& pathname)
{
    objl::Loader loader;
    loader.LoadFile(pathname);
    std::vector<Material> materials;
    std::vector<Mesh> toReturn;
    for (auto mat : loader.LoadedMaterials)
    {
        Material temp;
        temp.Ka = glm::vec3(mat.Ka.X,
            mat.Ka.Y,
            mat.Ka.Z);
        temp.Kd = glm::vec3(mat.Kd.X,
            mat.Kd.Y,
            mat.Kd.Z);
        temp.Ks = glm::vec3(mat.Ks.X,
            mat.Ks.Y,
            mat.Ks.Z);
        temp.shininess = mat.Ns;
        temp.name = mat.name;
        if(mat.map_Kd!="")
        temp.textureID = TextureUtilites::LoadTexture(mat.map_Kd);
        materials.push_back(temp);
    }
        
    std::vector<unsigned int> indices;
    for (int k = 0; k < loader.LoadedMeshes.size(); k++)
    {
        indices = loader.LoadedMeshes[k].Indices;
        std::vector<Vertex> vertices;
        for (int i = 0; i < loader.LoadedMeshes[k].Vertices.size(); i++)
        {
            Vertex temp;
            temp.Position = glm::vec3(loader.LoadedMeshes[k].Vertices[i].Position.X,
                loader.LoadedMeshes[k].Vertices[i].Position.Y,
                loader.LoadedMeshes[k].Vertices[i].Position.Z);
            temp.Normal = glm::vec3(loader.LoadedMeshes[k].Vertices[i].Normal.X,
                loader.LoadedMeshes[k].Vertices[i].Normal.Y,
                loader.LoadedMeshes[k].Vertices[i].Normal.Z);
            temp.TexCoords = glm::vec2(loader.LoadedMeshes[k].Vertices[i].TextureCoordinate.X,
                loader.LoadedMeshes[k].Vertices[i].TextureCoordinate.Y);
            vertices.push_back(temp);
        }
        Mesh mesh(vertices, indices);
        mesh.vertices = vertices;
        mesh.indices = indices;
        for (int j = 0; j < materials.size(); j++)
            if (materials[j].name == loader.LoadedMeshes[k].MeshMaterial.name)
                mesh.MeshMaterial = materials[j];
        toReturn.push_back(mesh);
    }
    return toReturn;
}
