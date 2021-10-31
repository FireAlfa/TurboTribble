#pragma once

#include "Component.h"

//#include "MathGeoLib/include/Geometry/AABB.h"
//#include "MathGeoLib/include/Geometry/OBB.h"

#include <vector>
typedef unsigned int uint;

class Mesh : Component
{
public:
	Mesh() { Mesh::type = CompType::MESH; }
	~Mesh() {}




public:


	unsigned int numIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
	unsigned int materialIndex;
};