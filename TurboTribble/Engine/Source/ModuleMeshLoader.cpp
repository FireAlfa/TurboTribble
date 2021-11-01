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
	TTLOG("+++++ Init Module MeshLoader +++++");

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

MeshInfo ModuleMeshLoader::LoadMesh(const char* filePath)
{	

	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		InitFromScene(scene, filePath);
		CreateBuffers();
		
		aiReleaseImport(scene);
		TTLOG("Mesh with path %s loaded successfully.", filePath);
	}
	else
	{
		TTLOG("Error loading scene '%s' : '%s' \n ", filePath);
	}

	return info;
}

void ModuleMeshLoader::InitFromScene(const aiScene* scene, const char* filePath)
{
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		const aiMesh* aiMesh = scene->mMeshes[i];
		InitSingleMesh(i, aiMesh);
	}
}

void ModuleMeshLoader::InitSingleMesh(unsigned int index, const aiMesh* aiMesh)
{
	// Copy vertices
	info.numVertex = aiMesh->mNumVertices;
	info.vertex = new float3[info.numVertex];
	memcpy(info.vertex, aiMesh->mVertices, sizeof(float) * info.numVertex * 3);
	TTLOG("New mesh with %d vertices", info.numVertex);

	// Copy faces
	if (aiMesh->HasFaces())
	{
		info.numIndex = aiMesh->mNumFaces * 3;
		info.index = new uint[info.numIndex * 2]; // assume each face is a triangle
		for (uint i = 0; i < aiMesh->mNumFaces; ++i)
		{
			if (aiMesh->mFaces[i].mNumIndices != 3)
			{
				TTLOG("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&info.index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
		TTLOG("Faces loaded");
	}

	// Copy texture coordinates
	if (aiMesh->HasTextureCoords(info.idTexCo))
	{
		info.texCo = new float2[info.numVertex];
		memcpy(info.texCo, aiMesh->mVertices, sizeof(float) * info.numTexCo * 3);
		TTLOG("Texture coordinates loaded");
	}


}

void ModuleMeshLoader::CreateBuffers()
{
	// Vertices buffer
	glGenBuffers(1, &info.idVertex);
	glBindBuffer(GL_ARRAY_BUFFER, info.idVertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * info.numVertex* 3, info.vertex, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 1);

	// Texture coords buffer
	glGenBuffers(1, &info.idTexCo);
	glBindBuffer(GL_ARRAY_BUFFER, info.idTexCo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * info.numTexCo * 2, info.texCo, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 1);

	// Indices buffer
	glGenBuffers(1, &info.idIndex);
	glBindBuffer(GL_ARRAY_BUFFER, info.idIndex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * info.numIndex, info.index, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 1);
}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	TTLOG("+++++ Quitting Module MeshLoader +++++");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}