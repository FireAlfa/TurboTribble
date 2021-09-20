#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"
#include "Primitive.h"
#include "glmath.h"

class btRigidBody;
class Module;

// =================================================
struct BuildingInfo
{
	vec3 pos;
	vec3 size;
	bool axis;
};

// =================================================
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);

	void SetAsSensor(bool is_sensor);
	bool GetIsSensor() { return isSensor; }

	btRigidBody* GetBody();

private:

	btRigidBody* body = nullptr;
	bool isSensor;

public:

	Cube cube;
	p2List<Module*> collision_listeners;
	bool isDeliver = false;
};

#endif // __PhysBody3D_H__