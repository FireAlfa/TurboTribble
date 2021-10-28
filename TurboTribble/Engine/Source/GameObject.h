#pragma once

#include "Component.h"

#include <vector>
#include <string>

using namespace std;

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
	string name;
	vector <Component> components;

	GameObject* parent;
	vector <GameObject*> children;
};