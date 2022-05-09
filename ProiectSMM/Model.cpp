#include "Model.h"

void Model::Draw(Shader& shader)
{
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}
