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

bool ModuleMeshLoader::LoadMesh(const char* filePath)
{	
	bool ret = false;


	const aiScene* scene = aiImportFile(filePath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr)
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiReleaseImport(scene);
		TTLOG("Mesh with path %s loaded successfully.", filePath);
	}
	else
		TTLOG("Error loading scene '%s' : '%s' \n ", filePath);

	return ret;
}

bool ModuleMeshLoader::InitFromScene(const aiScene* scene, const char* filePath)
{
	mMeshes.resize(scene->mNumMeshes);

	CountVerticesAndIndices(scene, numVertex, numIndex);

	ReserveSpace(numVertex, numIndex);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < mMeshes.size(); i++) {
		const aiMesh* aiMesh = scene->mMeshes[i];
		InitSingleMesh(i, aiMesh);
	}

	return true;
}

void ModuleMeshLoader::CountVerticesAndIndices(const aiScene* scene, uint numVertex, uint numIndex)
{
	for (uint i = 0; i < mMeshes.size(); i++)
	{
		mMeshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
		mMeshes[i].numIndices = scene->mMeshes[i]->mNumFaces * 3;
		mMeshes[i].baseVertex = numVertex;
		mMeshes[i].baseIndex = numIndex;
	}
}

void ModuleMeshLoader::ReserveSpace(uint numVertex, uint numIndex)
{
	mVertices.reserve(numVertex);
	mTextureCoords.reserve(numVertex);
	mNormals.reserve(numVertex);
	mIndices.reserve(numIndex);
}

void ModuleMeshLoader::InitSingleMesh(unsigned int index, const aiMesh* aiMesh)
{
	const aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (uint i = 0; i < aiMesh->mNumVertices; i++)
	{
		const aiVector3D& vecPos = aiMesh->mVertices[i];
		const aiVector3D& vecNormal = aiMesh->mNormals[i];
		const aiVector3D& vecTexCoords = aiMesh->HasTextureCoords(0) ? aiMesh->mTextureCoords[0][i] : zero3D;

		mVertices.push_back(vec3(vecPos.x, vecPos.y, vecPos.z));
		mNormals.push_back(vec3(vecNormal.x, vecNormal.y, vecNormal.z));
		mTextureCoords.push_back(vec2(vecTexCoords.x, vecTexCoords.y));
	}

	// Populate the index buffer
	for (uint i = 0; i < aiMesh->mNumFaces; i++)
	{
		const aiFace& face = aiMesh->mFaces[i];
		//assert(face.mNumIndices == 3);
		mIndices.push_back(face.mIndices[0]);
		mIndices.push_back(face.mIndices[1]);
		mIndices.push_back(face.mIndices[2]);
	}
}

void ModuleMeshLoader::FillBuffers()
{
	// Vertices buffer
	glGenBuffers(1, &mBuffers[VRTX_BUFF]);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[VRTX_BUFF]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mVertices) * mVertices.size(), &mVertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture coords buffer
	glGenBuffers(1, &mBuffers[TEXCOORD_BUFF]);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[TEXCOORD_BUFF]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mTextureCoords) * mTextureCoords.size(), &mTextureCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals buffer
	glGenBuffers(1, &mBuffers[NORMAL_BUFF]);
	glBindBuffer(GL_ARRAY_BUFFER, mBuffers[NORMAL_BUFF]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mNormals) * mNormals.size(), &mNormals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Indices buffer
	glGenBuffers(1, &mBuffers[VRTX_BUFF]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBuffers[INDEX_BUFF]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0]) * mIndices.size(), &mIndices[0], GL_STATIC_DRAW);

}

// Called before quitting
bool ModuleMeshLoader::CleanUp()
{
	TTLOG("+++++ Quitting Module MeshLoader +++++");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}