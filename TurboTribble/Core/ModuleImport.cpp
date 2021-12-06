#include "ModuleImport.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentTransform.h"
#include "GameObject.h"

#include "Globals.h"

#include <vector>
#include <queue>
#include "SDL/include/SDL_opengl.h"
#include "Math/float2.h"
// Assimp
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/mesh.h"



ModuleImport::ModuleImport(Application* app, bool startEnabled) : Module(app, startEnabled) {}

// Called before render is available
bool ModuleImport::Init()
{
	TTLOG("+++++ Loading Import Module +++++\n");
	bool ret = true;

	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	return ret;
}

bool ModuleImport::LoadGeometry(const char* path)
{
	// Own structure	
	GameObject* root = nullptr;
	std::string newRootName(path);

	// Assimp stuff
	aiMesh* assimpMesh = nullptr;
	const aiScene* scene = nullptr;
	aiMaterial* texture = nullptr;
	aiString texturePath;

	// Create path buffer and import to scene
	char* buffer = nullptr;
	uint bytesFile = app->fileSystem->Load(path, &buffer);

	if (buffer == nullptr)
	{
		std::string normPathShort = "Assets/Models/" + app->fileSystem->SetNormalName(path);
		app->fileSystem->NormalizePath(normPathShort.c_str());
		bytesFile = app->fileSystem->Load(normPathShort.c_str(), &buffer);
	}
	if (buffer != nullptr)
	{
		scene = aiImportFileFromMemory(buffer, bytesFile, aiProcessPreset_TargetRealtime_MaxQuality, NULL);
	}
	else
	{
		scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	}


	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (size_t i = 0; i < scene->mNumMeshes; i++)
		{		
			bool nameFound = false;
			std::string name;
			FindNodeName(scene, i, name);

			GameObject* newGameObject = app->scene->CreateGameObject(name);
			ComponentMesh* mesh = newGameObject->CreateComponent<ComponentMesh>();
			assimpMesh = scene->mMeshes[i];

			ProcessNode(scene->mMeshes[i], scene->mRootNode, scene->mRootNode->mTransformation, scene);
			
			if (scene->HasMaterials())
			{
				texture = scene->mMaterials[assimpMesh->mMaterialIndex];

				if (texture != nullptr)
				{
					texture->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);
					std::string newPath(texturePath.C_Str());
					std::string fileStr, extStr;
					app->fileSystem->SplitFilePath(newPath.c_str(), nullptr, &fileStr, &extStr);
					newPath = fileStr.append(".") + extStr;
					if (newPath.size() > 0)
					{
						mesh->texturePath = "Assets/Textures/" + newPath;
						if (!app->textures->Find(mesh->texturePath))
						{
							const TextureObject& textureObject = app->textures->Load(mesh->texturePath);							
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);
						}
						else
						{
							const TextureObject& textureObject = app->textures->Get(mesh->texturePath);
							ComponentMaterial* materialComp = newGameObject->CreateComponent<ComponentMaterial>();
							materialComp->SetTexture(textureObject);
						}
					}
				}
			}
	
			mesh->numVertices = assimpMesh->mNumVertices;
			mesh->vertices.resize(assimpMesh->mNumVertices);
			
			memcpy(&mesh->vertices[0], assimpMesh->mVertices, sizeof(float3) * assimpMesh->mNumVertices);
			TTLOG("+++ New mesh with %d vertices +++\n", assimpMesh->mNumVertices);

			// Copying faces
			if (assimpMesh->HasFaces())
			{
				mesh->numIndices = assimpMesh->mNumFaces * 3;
				mesh->indices.resize(mesh->numIndices);

				for (size_t i = 0; i < assimpMesh->mNumFaces; i++)
				{
					if (assimpMesh->mFaces[i].mNumIndices != 3)
					{
						TTLOG("### WARNING, geometry face with != 3 indices! ###\n")
					}
					else
					{
						memcpy(&mesh->indices[i * 3], assimpMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			
			// Copying Normals info
			if (assimpMesh->HasNormals())
			{

				mesh->normals.resize(assimpMesh->mNumVertices);
				memcpy(&mesh->normals[0], assimpMesh->mNormals, sizeof(float3) * assimpMesh->mNumVertices);
			}
			
			// Copying UV info
			if (assimpMesh->HasTextureCoords(0))
			{
				mesh->texCoords.resize(assimpMesh->mNumVertices);
				for (size_t j = 0; j < assimpMesh->mNumVertices; ++j)
				{
					memcpy(&mesh->texCoords[j], &assimpMesh->mTextureCoords[0][j], sizeof(float2));
				}
			}
			
			mesh->GenerateBuffers();
			mesh->GenerateBounds();
			mesh->ComputeNormals();


			// Place houses where to their pose
			aiVector3D pos, sc;
			aiQuaternion rot;
			// Decompose the transform mat
			scene->mRootNode->mTransformation.Decompose(sc, rot, pos);
			// Set the pos, rot and scale
			newGameObject->transform->SetPosition(float3(pos.x, pos.y, pos.z));
			newGameObject->transform->SetRotation(Quat(rot.x, rot.y, rot.z, rot.w).ToEulerXYZ());
			newGameObject->transform->SetScale(float3(sc.x, sc.y, sc.z));

		}
		aiReleaseImport(scene);		
		RELEASE_ARRAY(buffer);

	}
	else 
		TTLOG("### Error loading scene %s ###\n", path);

	RELEASE_ARRAY(buffer);

	return true;
}

void ModuleImport::FindNodeName(const aiScene* scene, const size_t i, std::string& name)
{
	bool nameFound = false;
	std::queue<aiNode*> Q;
	Q.push(scene->mRootNode);
	while (!Q.empty() && !nameFound)
	{
		aiNode* node = Q.front();
		Q.pop();
		for (size_t j = 0; j < node->mNumMeshes; ++j)
		{
			if (node->mMeshes[j] == i)
			{
				nameFound = true;
				name = node->mName.C_Str();
			}
		}
		if (!nameFound)
		{
			for (size_t j = 0; j < node->mNumChildren; ++j)
			{
				Q.push(node->mChildren[j]);
			}
		}
	}
}

void ModuleImport::ProcessNode(const aiMesh* mesh, aiNode* node, aiMatrix4x4& matrix, const aiScene* scene)
{
	for (int i = 0; i < node->mNumChildren; i++)
	{
		if (node->mChildren[i]->mNumMeshes > 0)
		{
			for (int j = 0; j < node->mChildren[i]->mNumMeshes; j++)
			{
				if (scene->mMeshes[node->mChildren[i]->mMeshes[j]] == mesh)
				{
					matrix = node->mChildren[i]->mTransformation;
				}
			}
		}
		if (node->mChildren[i]->mNumChildren > 0)
		{
			ProcessNode(mesh, node->mChildren[i], matrix, scene);
		}
	}
}

// Called before quitting
bool ModuleImport::CleanUp()
{
	TTLOG("+++++ Quitting Import Module +++++\n");

	// Detach log stream
	aiDetachAllLogStreams();

	return true;
}