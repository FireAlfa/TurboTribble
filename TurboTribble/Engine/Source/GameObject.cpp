#include "GameObject.h"

#include <string>

// Constructor
GameObject::GameObject(std::string* name)
{
	//name = new string(this);
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