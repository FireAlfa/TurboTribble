#include "GameObject.h"
#include "ModuleMeshLoader.h"

#include "Globals.h"

#include "CMesh.h"


// Constructor
GameObject::GameObject(const char* name, GameObject* parent)
{
	this->name = name;
	this->parent = parent;
	this->visible = false;
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

	switch (type)
	{
	case CompType::TRANSFORM:
		
		break;
	case CompType::MESH:
		ret = new Mesh(this);
		TTLOG("Added Mesh Component to %s", this->name.c_str());
		break;
	case CompType::MATERIAL:
		break;
	case CompType::LIGHT:
		break;
	default:
		break;
	}

	if (ret != nullptr)
	{
		components.push_back(ret);
	}

	return ret;
}

Component* GameObject::GetComponent(CompType type)
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components.at(i)->GetCompType() == type)
		{
			return components.at(i);
		}
	}

	return nullptr;
}

void GameObject::SetMeshInfo(MeshInfo meshInfo)
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components.at(i)->GetCompType() == CompType::MESH)
		{
			this->components.at(i)->SetMeshInfo(meshInfo);
		}
	}
}

void GameObject::Draw()
{
	MeshInfo meshInfo;

	for (int i = 0; i < components.size(); i++)
	{
		if (components.at(i)->GetCompType() == CompType::MESH)
		{
			meshInfo = this->components.at(i)->GetMeshInfo();
		}
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, meshInfo.idVertex);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, meshInfo.idTexCo);
	glVertexPointer(2, GL_FLOAT, 0, NULL);

	glBindTexture(GL_TEXTURE_2D, meshInfo.idTexture);
	
	glBindTexture(GL_ELEMENT_ARRAY_BUFFER, meshInfo.idIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.idIndex);

	glDrawElements(GL_TRIANGLES, meshInfo.numIndex, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}