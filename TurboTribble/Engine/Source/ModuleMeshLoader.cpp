#include "Application.h"
#include "ModuleMeshLoader.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"

// Constructor
ModuleMeshLoader::ModuleMeshLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
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
	TTLOG("+++++ Init Module MeshLoader +++++");

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

// PreUpdate: clear buffer
UpdateStatus ModuleMeshLoader::PreUpdate(float dt)
{

	return UpdateStatus::UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UpdateStatus ModuleMeshLoader::PostUpdate(float dt)
{
	return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	TTLOG("+++++ Quitting Module MeshLoader +++++");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}
