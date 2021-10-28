#pragma once

#include "GameObject.h" 

class GameObject;

enum Type
{
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
};

class Component
{
public:

	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	virtual void Update();

	virtual bool IsEnabled() { return active; }

	virtual bool GetOwner() { return owner; }

private:

	Type type;
	bool active;
	GameObject* owner;
};