#ifndef __GAMEOBJECT_H__
#define __GAMEOBJECT_H__

#include "Component.h"

#include <vector>
#include <string>



class GameObject
{
public:

	GameObject(const char* name, GameObject* parent);
	~GameObject();

	void Update();

	Component* AddComponent(CompType type);
	Component* GetComponent(CompType type);

private:

	// ----- GameObject's Variables -----

	std::string name;
	GameObject* parent = nullptr;
	bool active = true;
	// --------------------------------

	// ----- GameObject's Components -----

	std::vector<Component> components;
	// -----------------------------------

	// ----- GameObject's Children -----

	std::vector<GameObject*> children;
	// -----------------------------------

};

#endif // !__GAMEOBJECT_H__