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

	

	// Called before quitting, destroys the mesh loader
	bool CleanUp();

public:

	std::vector<vec3> mVertices;
	std::vector<vec2> mTextureCoords;
	std::vector<vec3> mNormals;

	std::vector<Mesh> mMeshes;

	uint idIndex = 0; // index in VRAM
	uint numIndex = 0;
	uint* index = nullptr;
	uint idVertex = 0; // unique vertex in VRAM
	uint numVertex = 0;
	float* vertex = nullptr;
};

#endif // !__MODULE_MESH_LOADER_H__