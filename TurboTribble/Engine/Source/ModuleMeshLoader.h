#ifndef __MODULE_MESH_LOADER_H__
#define __MODULE_MESH_LOADER_H__

#include "Application.h"
#include "Module.h"
#include "Globals.h"

#include "CMesh.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/Importer.hpp"
#include "glmath.h"
#include "glew.h"

class ModuleMeshLoader : public Module
{
public:
	// Constructor
	ModuleMeshLoader(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleMeshLoader();

	// Initialize the Mesh Loader
	bool Init();

	// Loads a Mesh by the filePath
	bool LoadMesh(const std::string& filePath);

	bool InitFromScene(const aiScene* scene, const std::string& filePath);

	void CountVerticesAndIndices(const aiScene* scene, uint numVertex, uint numIndex);

	void ReserveSpace(uint numVertex, uint numIndex);

	void InitSingleMesh(unsigned int index, const aiMesh* aiMesh);

	void FillBuffers();

	// Called before quitting, destroys the mesh loader
	bool CleanUp();

public:

	std::vector<vec3> mVertices;
	std::vector<vec2> mTextureCoords;
	std::vector<vec3> mNormals;
	std::vector<vec3> mIndices;

	std::vector<Mesh> mMeshes;

	uint numIndex = 0;
	uint* index = nullptr;
	uint numVertex = 0;
	float* vertex = nullptr;

	GLuint mBuffers[4]{ 0 };
};


enum BufferType
{
	INDEX_BUFF = 0,
	VRTX_BUFF = 1,
	TEXCOORD_BUFF = 2,
	NORMAL_BUFF = 3,
};
#endif // !__MODULE_MESH_LOADER_H__