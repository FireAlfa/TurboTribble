#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "p2List.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 17500.0f
#define TURN_DEGREES 25.0f * DEGTORAD
#define BRAKE_POWER 1400.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
	void GenerateDeliveryPoint();
	void GenerateCollectPoint();

	void SetInitPos();

	enum Gear
	{
		PARKING = 0,
		DRIVE,
		REVERSE
	};

public:

	Gear gearState = PARKING;
	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;
	bool engine = true;
	bool map = false;
	bool radio = true;

	bool isAccel = false;
	bool isDesaccel = true;

	btVector3* playerDeliveryPoint = nullptr;
	btVector3* playerCollectPoint = nullptr;

	int firstPos;
	bool wasDelivered = false;

	uint gearFx;
	uint engineFx;
	uint engineOnFx;
	uint engineOffFx;
	uint engineAccelerationFx;
	uint lockFx;
	uint deliveryFx;
	uint reverseFx;
};