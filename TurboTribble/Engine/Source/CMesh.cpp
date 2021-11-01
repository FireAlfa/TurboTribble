#include "CMesh.h"

Mesh::Mesh()
{
	
} 

Mesh::~Mesh()
{

}

void Mesh::SetMeshInfo(MeshInfo mI)
{
	/*compMeshInfo.idIndex = mI.idIndex;
	compMeshInfo.idTexCo = mI.idTexCo;
	compMeshInfo.idVertex = mI.idVertex;
	compMeshInfo.index = mI.index;
	for (int i = 0; i < 4; i++)
	{
		compMeshInfo.mBuffers[i] = mI.mBuffers[i];
	}
	compMeshInfo.mIndices = mI.mIndices;
	compMeshInfo.mNormals = mI.mNormals;
	compMeshInfo.mTextureCoords = mI.mTextureCoords;
	compMeshInfo.mVertices = mI.mVertices;
	compMeshInfo.numIndex = mI.numIndex;
	compMeshInfo.numTexCo = mI.numTexCo;
	compMeshInfo.numVertex = mI.numVertex;
	compMeshInfo.texCo = mI.texCo;
	compMeshInfo.vertex = mI.vertex;*/
	this->meshType = meshType;
}