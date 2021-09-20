#pragma once
#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "p2List.h"

#define MAX_SNAKE 2
#define TILE_SIZE 10.0f

struct PhysBody3D;
struct PhysMotor3D;
struct PhysVehicle3D;
class btVector3;
class btGeneric6DofConstraint;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void CreateBuilding(float x, float y, float z, vec3 size, bool axis, Color color = cBuilding);
	void CreateTrailer(float x, float y, float z, int id);

	void SetTrailerPos(float x, float y, float z, int id);

	void LooseCondition();

	void DefineDeliveryPoints();
	void CreateDeliverySensor();

public:
	
	p2List<Cube> buildings;

public:
	

	p2DynArray<btVector3> deliveryPoints;

	btGeneric6DofConstraint* cs;

	Cylinder tree;
	Sphere leaf;
	Cylinder tree1;
	Sphere leaf1;
	Cylinder tree2;
	Sphere leaf2;
	Cylinder tree3;
	Sphere leaf3;
	Cylinder tree4;
	Sphere leaf4;
	Cylinder tree5;
	Sphere leaf5;

	p2DynArray<PhysBody3D*> deliverySensor;
	bool isJoint = false;

	PhysVehicle3D* remolque;
};
