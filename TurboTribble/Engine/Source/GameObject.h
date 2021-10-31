#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"

#include <vector>
#include <string>

//using namespace std;

class GameObject
{
public:
	GameObject(const char* name);
	~GameObject();

	Component* CreateComponent(CompType type);
	Component* GetComponent(CompType type);

	void Update();

private:
	bool active = true;
	std::string name;

	std::vector<Component> components;
	GameObject* parent;
	std::vector<GameObject*> children;
};

#endif // !__GAMEOBJECT_H__