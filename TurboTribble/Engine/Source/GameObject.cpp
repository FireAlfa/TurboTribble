#include "GameObject.h"
#include "ModuleMeshLoader.h"

#include "Globals.h"


// Constructor
GameObject::GameObject(const char* name, GameObject* parent)
{
	this->name = name;
	this->parent = parent;
	if (this->parent != nullptr)
	{
		parent->children.push_back(this);
		TTLOG("GameObject %s created successfully. Child of %s", name, parent->name.c_str());
	}

	// TODO Apply transform
}

// Destructor
GameObject::~GameObject()
{
}

void GameObject::Update()
{
}

Component* GameObject::AddComponent(CompType type)
{
	Component* ret = nullptr;

	return ret;
}

Component* GameObject::GetComponent(CompType type)
{
	Component* ret = nullptr;

	return ret;
}

void GameObject::SetMeshInfo(MeshInfo meshInfo)
{
	Component* comp = GetComponent(CompType::MESH);

	if (comp != nullptr) { comp->SetMeshInfo(meshInfo); };
}

void GameObject::Draw()
{
	Component* mesh = GetComponent(CompType::MESH);

	//MeshInfo* meshInfo = mesh->GetMeshInfo();

	glEnableClientState(GL_VERTEX_ARRAY);

	//glBindBuffer(GL_ARRAY_BUFFER, app->loader->mBuffers[VRTX_BUFF]);
}