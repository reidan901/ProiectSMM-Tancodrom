#pragma once
#include <string>
//#include <tiny_obj_loader.h>
#include <iostream>
#include <glm.hpp>
#include "Mesh.h"
class Loader
{
public:
	static std::vector<Mesh> LoadObj(std::string& pathname);
};