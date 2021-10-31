#include "GameObject.h"

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

Component* CreateComponent(CompType type)
{
	Component* ret = nullptr;

	return ret;
}

Component* GetComponent(CompType type)
{
	Component* ret = nullptr;

	return ret;
}