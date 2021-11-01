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

	glGenBuffers(1, &info.mBuffers[VRTX_BUFF]);
	glGenBuffers(1, &info.mBuffers[TEXCOORD_BUFF]);
	glGenBuffers(1, &info.mBuffers[NORMAL_BUFF]);
	glGenBuffers(1, &info.mBuffers[INDEX_BUFF]);

	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		InitFromScene(scene, filePath);
		//FillBuffers();
		
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

//void ModuleMeshLoader::FillBuffers()
//{
//	// Vertices buffer
//	glEnableClientState(GL_VERTEX_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[VRTX_BUFF]);
//	glVertexPointer(3, GL_FLOAT, 0, NULL);
//
//	// Texture coords buffer
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TEXCOORD_BUFF]);
//	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
//
//	// Normals buffer
//	glEnableClientState(GL_NORMAL_ARRAY);
//	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[NORMAL_BUFF]);
//	glNormalPointer(GL_FLOAT, 0, NULL);
//
//	// Indices buffer
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[INDEX_BUFF]);
//
//	glDrawElements(GL_TRIANGLES, info.mIndices.size(), GL_UNSIGNED_INT, NULL);
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	TTLOG("+++++ Quitting Module MeshLoader +++++");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}