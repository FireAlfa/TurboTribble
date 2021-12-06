#ifndef __MODULE_IMPORT_H__
#define __MODULE_IMPORT_H__

#include "Module.h"
#include "Assimp/include/matrix4x4.h"

#include <string>

struct aiMesh;
struct aiNode;
struct aiScene;


class ComponentMesh;
struct aiScene;

class ModuleImport : public Module
{
public:

	// Constructor
	ModuleImport(Application* app, bool startEnabled = true);

	// Initialize the File Importer
	bool Init() override;
	// Called before quitting
	bool CleanUp() override;


	// Load a Geometry from a given path
	bool LoadGeometry(const char* path);
	// Find nodw in given scene
	void FindNodeName(const aiScene* scene, const size_t i, std::string& name);

	void ProcessNode(const aiMesh* mesh, aiNode* node, aiMatrix4x4& matrix, const aiScene* scene);

	void SetScene();
	void Resize();

};

#endif // !__MODULE_IMPORT_H__