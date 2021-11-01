#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include "ModuleMeshLoader.h"
#include "imgui/imgui.h"



class GameObject;

enum class CompType
{
	EMPTY = 0,
	TRANSFORM,
	MESH,
	MATERIAL,
	LIGHT,
};

class Component
{
public:

	Component() {}
	Component(GameObject* owner) : active(true), owner(owner), type(CompType::EMPTY) {}
	virtual ~Component() {}

	virtual void Enable() { active = true; }
	virtual void Disable() { active = false; }

	virtual void Update(float dt) {}


	virtual bool IsEnabled() { return active; }

	virtual bool GetOwner() { return owner; }

	virtual void SetMeshInfo(MeshInfo meshInfo) {}
	virtual MeshInfo GetMeshInfo() { return meshInfo; }

	virtual CompType GetCompType() { return type; }


	virtual void DrawInspector() {}

protected:

	MeshInfo meshInfo;

	CompType type;
	bool active;
	GameObject* owner;
};

#endif // !__COMPONENT_H__