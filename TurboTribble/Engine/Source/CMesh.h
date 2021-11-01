#ifndef __CMESH_H__
#define __CMESH_H__

#include "Globals.h"
#include "Component.h"
#include "ModuleMeshLoader.h"

#include <vector>

class ModuleMeshLoader;

 enum class MeshType
{
	CUBE,
	SPHERE,
	CYLINDER,
	FBX
};

class Mesh : Component
{
public:

	Mesh();
	~Mesh();

	void SetMeshInfo(MeshInfo meshInfo);

public:

	MeshInfo meshInfo;

	MeshType meshType;

	unsigned int numIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
	unsigned int materialIndex;
};
#endif // !__CMESH_H__
