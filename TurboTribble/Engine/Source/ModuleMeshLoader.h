#ifndef __MODULE_MESH_LOADER_H__
#define __MODULE_MESH_LOADER_H__

#include "Application.h"
#include "Module.h"
#include "Globals.h"

#include "glew.h"
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Math/float3.h"
#include "Math/float2.h"



struct MeshInfo
{
	uint idIndex = 0; // index in VRAM
	uint numIndex = 0;
	uint* index = nullptr;

	uint idVertex = 0; // unique vertex in VRAM
	uint numVertex = 0;
	float3* vertex = nullptr;

	uint idTexCo = 0; // unique vertex in VRAM
	uint numTexCo = 0;
	float2* texCo = nullptr;

};

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
	MeshInfo LoadMesh(const char* filePath);

	void InitFromScene(const aiScene* scene, const char* filePath);

	void InitSingleMesh(unsigned int index, const aiMesh* aiMesh);

	void CreateBuffers();

	// Called before quitting, destroys the mesh loader
	bool CleanUp();

public:

	MeshInfo info;

};
#endif // !__MODULE_MESH_LOADER_H__