#include "CMesh.h"

Mesh::Mesh(GameObject* owner)
{
	type = CompType::MESH;
	this->owner = owner;
} 

Mesh::~Mesh()
{

}

void Mesh::SetMeshInfo(MeshInfo mI)
{
	compMeshInfo.idIndex = mI.idIndex;
	compMeshInfo.idTexCo = mI.idTexCo;
	compMeshInfo.idVertex = mI.idVertex;
	compMeshInfo.index = mI.index;
	compMeshInfo.numIndex = mI.numIndex;
	compMeshInfo.numTexCo = mI.numTexCo;
	compMeshInfo.numVertex = mI.numVertex;
	compMeshInfo.texCo = mI.texCo;
	compMeshInfo.vertex = mI.vertex;
	//this->meshType = meshType;
}

void Mesh::DrawInspector()
{
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{

	}
}