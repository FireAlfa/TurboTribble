#pragma once

#include "Component.h"

#include "MathGeoLib/include/Geometry/AABB.h"
#include "MathGeoLib/include/Geometry/OBB.h"

#include <vector>
typedef unsigned int uint;

class Mesh : Component
{
public:
	Mesh() { Mesh::type = CompType::MESH; }
	~Mesh();




public:
	uint id_index = 0; // index in VRAM
	uint num_index = 0;
	uint* index = nullptr;
	uint id_vertex = 0; // unique vertex in VRAM
	uint num_vertex = 0;
	float* vertex = nullptr;

};