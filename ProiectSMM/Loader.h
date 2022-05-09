#pragma once
#include <string>
#include <tiny_obj_loader.h>
#include <iostream>
#include <glm/glm.hpp>
#include "Mesh.h"
class Loader
{
public:
	static void LoadObj(std::vector<Mesh>& meshes);
};

