#ifndef __CMESH_H__
#define __CMESH_H__

#include "Globals.h"
#include "Component.h"

#include <vector>



 enum class MeshType
{
	CUBE,
	SPHERE,
	CYLINDER,
	FBX
};

class Mesh : public Component
{
public:

	Mesh(GameObject* owner);
	~Mesh();

	void SetMeshInfo(MeshInfo meshInfo);

	MeshInfo GetMeshInfo() {
		return compMeshInfo; }

	void DrawInspector();

public:

	MeshInfo compMeshInfo;

	MeshType meshType;
	CompType compType;

	bool wireframe = false;
};
#endif // !__CMESH_H__
