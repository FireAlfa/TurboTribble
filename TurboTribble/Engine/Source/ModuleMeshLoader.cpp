#include "ModuleMeshLoader.h"

// Constructor
ModuleMeshLoader::ModuleMeshLoader(Application* app, bool startEnabled) : Module(app, startEnabled)
{

}

// Destructor
ModuleMeshLoader::~ModuleMeshLoader()
{
}

// Called before render is available
bool ModuleMeshLoader::Init()
{
	bool ret = true;
	TTLOG("Creating Mesh Loader context");

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

bool ModuleMeshLoader::LoadMesh(const std::string& filePath)
{	
	bool ret = false;

	Assimp::Importer Importer;

	const aiScene* scene = Importer.ReadFile(filePath.c_str(), ASSIMP_LOAD_FLAGS);
	if (scene)
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
	}
	else
		TTLOG("Error loading scene '%s' : '%s' \n ", filePath.c_str(), Importer.GetErrorString());

	return ret;
}

bool ModuleMeshLoader::InitFromScene(const aiScene* scene, const string& filePath)
{
	mMeshes.resize(scene->mNumMeshes);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < mMeshes.size(); i++) {
		const aiMesh* aiMesh = scene->mMeshes[i];
		//InitSingleMesh(i, aiMesh);
}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	TTLOG("Destroying Mesh Loader");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}