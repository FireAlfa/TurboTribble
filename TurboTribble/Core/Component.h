#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "GameObject.h"

#include <vector>
#include <string>



class Application;

class Component {

public:


	Component(GameObject* parent) : owner(parent)
	{
		if (parent)
			parent->AddComponent(this);
	};
	virtual ~Component() {};

	virtual bool Update(float dt) {
		return true;
	}

	virtual bool Enable() { return active = true; };
	virtual bool Disable() { return active = false; };
	virtual void OnGui() { }

public:

	GameObject* owner = nullptr;
	bool active = true;
};

#endif // !__COMPONENT_H__