#pragma once

#include "Component.h"

#include <vector>
#include <string>

class GameObject
{
public:
	GameObject(char* name);
	~GameObject();

	Component* AddComponent(Type type);
	Component* GetComponent(Type type);

	void Update();

private:
	bool active = true;
	std::string name;
	std::vector <Component> components;
};