#ifndef __CMESH_H__
#define __CMESH_H__

#include "Component.h"

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
#endif // !__CMESH_H__
