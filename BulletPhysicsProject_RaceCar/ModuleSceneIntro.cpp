#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	DefineDeliveryPoints();
	CreateDeliverySensor();

	//
	// Create buildings
	//
	{
		// Central grid
		float posZ = 0;
		float posX = 0;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				posZ = 4 * TILE_SIZE * i;
				posX = 4 * TILE_SIZE * j;
				CreateBuilding(2 * TILE_SIZE - (2 * TILE_SIZE / 2) - posX, 6 * TILE_SIZE, -2 * TILE_SIZE + (2 * TILE_SIZE / 2) + posZ, { 2 * TILE_SIZE, 6 * TILE_SIZE, 2 * TILE_SIZE }, false);
			}
		}

		// Left column
		for (int i = 0; i < 4; ++i)
		{
			posZ = 4 * TILE_SIZE * i;	
			CreateBuilding(14 * TILE_SIZE - (3 * TILE_SIZE / 2), 3 * TILE_SIZE, -2 * TILE_SIZE + (2 * TILE_SIZE / 2) + posZ, { 3 * TILE_SIZE, 3 * TILE_SIZE, 2 * TILE_SIZE }, false);
		}

		// City hall
		CreateBuilding(7 * TILE_SIZE - (TILE_SIZE / 2), TILE_SIZE , 9 * TILE_SIZE + (TILE_SIZE / 2), { TILE_SIZE, TILE_SIZE, TILE_SIZE }, false);
		CreateBuilding(9 * TILE_SIZE - (5 * TILE_SIZE / 2), 2 * TILE_SIZE, 10 * TILE_SIZE + (2 * TILE_SIZE / 2), { 5 * TILE_SIZE, 2 * TILE_SIZE, 2 * TILE_SIZE }, false);
		CreateBuilding(8 * TILE_SIZE - (3 * TILE_SIZE / 2), 1.5f * TILE_SIZE, 12 * TILE_SIZE + (TILE_SIZE / 2), { 3 * TILE_SIZE, 1.5f * TILE_SIZE, TILE_SIZE }, false);
		
		// City hall front building
		CreateBuilding(9 * TILE_SIZE - (5 * TILE_SIZE / 2), 2.5f * TILE_SIZE, -2 * TILE_SIZE + (2 * TILE_SIZE / 2), { 5 * TILE_SIZE, 2.5 * TILE_SIZE , 2 * TILE_SIZE }, false);
		
		// Right column
		for (int i = 0; i < 8; ++i)
		{
			posZ = 3 * TILE_SIZE * i;
			CreateBuilding(-16 * TILE_SIZE - TILE_SIZE, 2 * TILE_SIZE / 2.0f, -7 * TILE_SIZE + (2 * TILE_SIZE / 2) + posZ, {TILE_SIZE, 2 * TILE_SIZE / 2, 2 * TILE_SIZE }, false);
		}

		// Centre back row
		CreateBuilding(14 * TILE_SIZE - (3 * TILE_SIZE / 2), 4 * TILE_SIZE, -8 * TILE_SIZE + (4 * TILE_SIZE / 2), { 3 * TILE_SIZE, 4 * TILE_SIZE, 4 * TILE_SIZE }, false);
		CreateBuilding(9 * TILE_SIZE - (5 * TILE_SIZE / 2), 1.5f * TILE_SIZE, -8 * TILE_SIZE + (4 * TILE_SIZE / 2), { 5 * TILE_SIZE, 1.5f * TILE_SIZE, 4 * TILE_SIZE }, false);
		for (int i = 0; i < 2; ++i)
		{
			posX = 4 * TILE_SIZE * i;
			CreateBuilding(2 * TILE_SIZE - (2 * TILE_SIZE / 2) - posX, 4 * TILE_SIZE, -8 * TILE_SIZE + (4 * TILE_SIZE / 2), { 2 * TILE_SIZE, 4 * TILE_SIZE, 4 * TILE_SIZE }, false);
		}

		// Back row
		CreateBuilding(14 * TILE_SIZE - (4 * TILE_SIZE / 2), 3.5f * TILE_SIZE, -13 * TILE_SIZE + (3 * TILE_SIZE / 2), { 4 * TILE_SIZE, 3.5f * TILE_SIZE, 3 * TILE_SIZE }, false);
		for (int i = 0; i < 3; ++i)
		{
			posX = 5 * TILE_SIZE * i;
			CreateBuilding(9 * TILE_SIZE - (3 * TILE_SIZE / 2) - posX, 2.8f * TILE_SIZE, -13 * TILE_SIZE + (3 * TILE_SIZE / 2), { 3 * TILE_SIZE, 2.8f * TILE_SIZE, 3 * TILE_SIZE }, false);
		}
		CreateBuilding(-6 * TILE_SIZE - (3 * TILE_SIZE / 2), 4 * TILE_SIZE, -13 * TILE_SIZE + (4 * TILE_SIZE / 2), { 3 * TILE_SIZE, 4 * TILE_SIZE, 4 * TILE_SIZE }, false);
		CreateBuilding(-10 * TILE_SIZE - (2 * TILE_SIZE / 2), 2.5f * TILE_SIZE, -13 * TILE_SIZE + (3 * TILE_SIZE / 2), { 2 * TILE_SIZE, 2.5f * TILE_SIZE, 3 * TILE_SIZE }, false);
	}


	//
	// Create parks
	//
	{
		CreateBuilding(9 * TILE_SIZE - (5 * TILE_SIZE / 2), 0.2f, TILE_SIZE + (7 * TILE_SIZE / 2), { 5 * TILE_SIZE, 0.2f, 7 * TILE_SIZE }, false, cGrass);
		CreateBuilding(-6 * TILE_SIZE - (7 * TILE_SIZE / 2), 0.2f, -7 * TILE_SIZE + (4 * TILE_SIZE / 2), { 7 * TILE_SIZE, 0.2f, 4 * TILE_SIZE }, false, cGrass);

		// Park 1
		tree.color = cTree;
		tree.SetPos(8 * TILE_SIZE + TILE_SIZE / 2, 2.7f, 7 * TILE_SIZE + TILE_SIZE / 2);
		tree.SetRotation(90, vec3(0, 0, 1));
		tree.radius = 0.5;
		tree.height = 5;
		App->physics->AddBody(tree, 100000);

		leaf.color = cLeaf;
		leaf.SetPos(8 * TILE_SIZE + TILE_SIZE / 2, 6.0f, 7 * TILE_SIZE + TILE_SIZE / 2);
		leaf.SetRotation(90, vec3(0, 0, 1));
		leaf.radius = 3;

		tree1.color = cTree;
		tree1.SetPos(4 * TILE_SIZE + TILE_SIZE / 2, 2.7f, 3 * TILE_SIZE + TILE_SIZE / 2);
		tree1.SetRotation(90, vec3(0, 0, 1));
		tree1.radius = 0.5;
		tree1.height = 5;
		App->physics->AddBody(tree1, 100000);

		leaf1.color = cLeaf;
		leaf1.SetPos(4 * TILE_SIZE + TILE_SIZE / 2, 6.0f, 3 * TILE_SIZE + TILE_SIZE / 2);
		leaf1.SetRotation(90, vec3(0, 0, 1));
		leaf1.radius = 3;

		tree2.color = cTree;
		tree2.SetPos(8 * TILE_SIZE + TILE_SIZE / 2, 2.7f, 2 * TILE_SIZE + TILE_SIZE / 2);
		tree2.SetRotation(90, vec3(0, 0, 1));
		tree2.radius = 0.5;
		tree2.height = 5;
		App->physics->AddBody(tree2, 100000);

		leaf2.color = cLeaf;
		leaf2.SetPos(8 * TILE_SIZE + TILE_SIZE / 2, 6.0f, 2 * TILE_SIZE + TILE_SIZE / 2);
		leaf2.SetRotation(90, vec3(0, 0, 1));
		leaf2.radius = 3;

		// Park 2

		tree3.color = cTree;
		tree3.SetPos(-10 * TILE_SIZE + TILE_SIZE / 2, 2.7f, -5 * TILE_SIZE + TILE_SIZE / 2);
		tree3.SetRotation(90, vec3(0, 0, 1));
		tree3.radius = 0.5;
		tree3.height = 5;
		App->physics->AddBody(tree3, 100000);

		leaf3.color = cLeaf;
		leaf3.SetPos(-10 * TILE_SIZE + TILE_SIZE / 2, 6.0f, -5 * TILE_SIZE + TILE_SIZE / 2);
		leaf3.SetRotation(90, vec3(0, 0, 1));
		leaf3.radius = 3;

		tree4.color = cTree;
		tree4.SetPos(-7 * TILE_SIZE + TILE_SIZE / 2, 2.7f, -6 * TILE_SIZE + TILE_SIZE / 2);
		tree4.SetRotation(90, vec3(0, 0, 1));
		tree4.radius = 0.5;
		tree4.height = 5;
		App->physics->AddBody(tree4, 100000);

		leaf4.color = cLeaf;
		leaf4.SetPos(-7 * TILE_SIZE + TILE_SIZE / 2, 6.0f, -6 * TILE_SIZE + TILE_SIZE / 2);
		leaf4.SetRotation(90, vec3(0, 0, 1));
		leaf4.radius = 3;

		tree5.color = cTree;
		tree5.SetPos(-12 * TILE_SIZE + TILE_SIZE / 2, 2.7f, -7 * TILE_SIZE + TILE_SIZE / 2);
		tree5.SetRotation(90, vec3(0, 0, 1));
		tree5.radius = 0.5;
		tree5.height = 5;
		App->physics->AddBody(tree5, 100000);

		leaf5.color = cLeaf;
		leaf5.SetPos(-12 * TILE_SIZE + TILE_SIZE / 2, 6.0f, -7 * TILE_SIZE + TILE_SIZE / 2);
		leaf5.SetRotation(90, vec3(0, 0, 1));
		leaf5.radius = 3;
	}


	//
	// Create roundabouts
	//
	{
		CreateBuilding(0 * TILE_SIZE - (2 * TILE_SIZE / 2), 0.5f, 13 * TILE_SIZE + (2 * TILE_SIZE / 2), { 2 * TILE_SIZE, 0.5f, 2 * TILE_SIZE }, false, cRoundabout);
		CreateBuilding(-13 * TILE_SIZE - (3 * TILE_SIZE / 2), 0.5f, -13 * TILE_SIZE + (3 * TILE_SIZE / 2), { 3 * TILE_SIZE, 0.5f, 3 * TILE_SIZE }, false, cRoundabout);
		CreateBuilding(-14 * TILE_SIZE - (TILE_SIZE / 2), 3.0f, -12 * TILE_SIZE + (TILE_SIZE / 2), { TILE_SIZE / 2, 2.0f, TILE_SIZE / 2 }, false, cStatue);
	}

	return ret;
}

void ModuleSceneIntro::DefineDeliveryPoints()
{
	//Every city point of deliver and collect defined
	deliveryPoints.PushBack(btVector3(13 * TILE_SIZE + 10, 0, TILE_SIZE * 10 - 7));
	deliveryPoints.PushBack(btVector3(4 * TILE_SIZE, 0, 11 * TILE_SIZE + 5));
	deliveryPoints.PushBack(btVector3(-4 * TILE_SIZE, 0, TILE_SIZE * 11 + 4));
	deliveryPoints.PushBack(btVector3(-16 * TILE_SIZE + 4, 0, 14 * TILE_SIZE + 10));
	deliveryPoints.PushBack(btVector3(-7 * TILE_SIZE - 4, 0, 4 * TILE_SIZE));
	deliveryPoints.PushBack(btVector3(13 * TILE_SIZE + 10, 0, 1 * TILE_SIZE));
	deliveryPoints.PushBack(btVector3(1 * TILE_SIZE - 0.1, 0, -10 * TILE_SIZE - 30));
	deliveryPoints.PushBack(btVector3(-9 * TILE_SIZE - 25, 0, TILE_SIZE * -10 + 1));
}

void ModuleSceneIntro::CreateDeliverySensor()
{
	for (int i = 0; i < deliveryPoints.Count(); i++)
	{
		Cube cubeSensor;
		cubeSensor.color = Blue;
		cubeSensor.SetPos(deliveryPoints[i].getX() - 3.5, deliveryPoints[i].getY() + 2.5, deliveryPoints[i].getZ() + 3.5);
		cubeSensor.size = vec3(7, 5, 7);

		deliverySensor.PushBack(App->physics->AddBuilding(cubeSensor, 100000));
		PhysBody3D* auxBody = *deliverySensor.At(i);
		auxBody->collision_listeners.add(this);
		auxBody->GetBody()->setUserPointer(auxBody);
	}
}

void ModuleSceneIntro::CreateTrailer(float x, float y, float z, int id)
{
	{
		VehicleInfo trailerInfo;

		// Trailer properties ---------------------------------------
		trailerInfo.chassis_size.Set(2.5f, 3, 9.f);
		trailerInfo.chassis_offset.Set(0, 0, 0);

		trailerInfo.mass = 250.0f;
		trailerInfo.suspensionStiffness = 15.88f;
		trailerInfo.suspensionCompression = 0.83f;
		trailerInfo.suspensionDamping = 0.88f;
		trailerInfo.maxSuspensionTravelCm = 1000.0f;
		trailerInfo.frictionSlip = 50.5;
		trailerInfo.maxSuspensionForce = 6000.0f;

		// Wheel properties ---------------------------------------
		float connection_height = 1.2f;
		float wheel_radius = 0.6f;
		float wheel_width = 0.5f;
		float suspensionRestLength = 1.2f;
		vec3 direction(0, -1, 0);
		vec3 axis(-1, 0, 0);

		float half_width_remo = trailerInfo.chassis_size.x * 0.5f;
		float half_length_remo = trailerInfo.chassis_size.z * 0.5f;

		trailerInfo.num_wheels = 4;
		trailerInfo.wheels = new Wheel[4];

		// FRONT-LEFT ------------------------
		trailerInfo.wheels[0].connection.Set(half_width_remo - 0.3f * wheel_width, connection_height * 0.1 - 0.5, -half_length_remo + wheel_radius + 1.5);
		trailerInfo.wheels[0].direction = direction;
		trailerInfo.wheels[0].axis = axis;
		trailerInfo.wheels[0].suspensionRestLength = suspensionRestLength;
		trailerInfo.wheels[0].radius = wheel_radius;
		trailerInfo.wheels[0].width = wheel_width;
		trailerInfo.wheels[0].front = false;
		trailerInfo.wheels[0].drive = false;
		trailerInfo.wheels[0].brake = false;
		trailerInfo.wheels[0].steering = false;

		// FRONT-RIGHT ------------------------
		trailerInfo.wheels[1].connection.Set(-half_width_remo + 0.3f * wheel_width, connection_height * 0.1 - 0.5, -half_length_remo + wheel_radius + 1.5);
		trailerInfo.wheels[1].direction = direction;
		trailerInfo.wheels[1].axis = axis;
		trailerInfo.wheels[1].suspensionRestLength = suspensionRestLength;
		trailerInfo.wheels[1].radius = wheel_radius;
		trailerInfo.wheels[1].width = wheel_width;
		trailerInfo.wheels[1].front = false;
		trailerInfo.wheels[1].drive = false;
		trailerInfo.wheels[1].brake = false;
		trailerInfo.wheels[1].steering = false;

		// REAR-LEFT ------------------------
		trailerInfo.wheels[2].connection.Set(half_width_remo - 0.3f * wheel_width, connection_height * 0.1 - 0.5, -half_length_remo + wheel_radius);
		trailerInfo.wheels[2].direction = direction;
		trailerInfo.wheels[2].axis = axis;
		trailerInfo.wheels[2].suspensionRestLength = suspensionRestLength;
		trailerInfo.wheels[2].radius = wheel_radius;
		trailerInfo.wheels[2].width = wheel_width;
		trailerInfo.wheels[2].front = false;
		trailerInfo.wheels[2].drive = false;
		trailerInfo.wheels[2].brake = false;
		trailerInfo.wheels[2].steering = false;

		// REAR-RIGHT ------------------------
		trailerInfo.wheels[3].connection.Set(-half_width_remo + 0.3f * wheel_width, connection_height * 0.1 - 0.5, -half_length_remo + wheel_radius);
		trailerInfo.wheels[3].direction = direction;
		trailerInfo.wheels[3].axis = axis;
		trailerInfo.wheels[3].suspensionRestLength = suspensionRestLength;
		trailerInfo.wheels[3].radius = wheel_radius;
		trailerInfo.wheels[3].width = wheel_width;
		trailerInfo.wheels[3].front = false;
		trailerInfo.wheels[3].drive = false;
		trailerInfo.wheels[3].brake = false;
		trailerInfo.wheels[3].steering = false;

		trailerInfo.color = cTrailer;

		remolque = App->physics->AddVehicle(trailerInfo);

		switch (id)
		{
		case 0:
			remolque->cube.transform.rotate(-90, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 13, y, z);
			break;
		case 1:
			remolque->cube.transform.rotate(180, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 4, y, z - 5);
			break;
		case 2:
			remolque->cube.transform.rotate(180, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 4, y, z - 5);
			break;
		case 3:
			remolque->cube.transform.rotate(180, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 3, y, z - 5);
			break;
		case 4:
			remolque->cube.transform.rotate(90, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x + 8, y, z + 4);
			break;
		case 5:
			remolque->cube.transform.rotate(-90, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 13, y, z + 4);
			break;
		case 6:
			remolque->cube.transform.rotate(0, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x - 4, y, z + 14);
			break;
		case 7:
			remolque->cube.transform.rotate(90, vec3(0, 1, 0));
			remolque->SetTransform(remolque->cube.transform.M);
			remolque->SetPos(x + 8, y, z + 4);
			break;
		default:
			break;
		}
		remolque->collision_listeners.add(this);
		remolque->GetBody()->setUserPointer(remolque);
	}
}

void  ModuleSceneIntro::SetTrailerPos(float x, float y, float z, int id)
{
	remolque->GetBody()->setLinearVelocity({ 0,0,0 });
	remolque->GetBody()->setAngularVelocity({ 0,0,0 });
	App->player->vehicle->GetBody()->setLinearVelocity({ 0,0,0 });
	App->player->vehicle->GetBody()->setAngularVelocity({ 0,0,0 });
	switch (id)
	{
	case 0:
		remolque->cube.transform.rotate(-90, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 13, y, z);
		break;
	case 1:
		remolque->cube.transform.rotate(180, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 4, y, z - 5);
		break;
	case 2:
		remolque->cube.transform.rotate(180, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 4, y, z - 5);
		break;
	case 3:
		remolque->cube.transform.rotate(180, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 3, y, z - 5);
		break;
	case 4:
		remolque->cube.transform.rotate(90, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x + 8, y, z + 4);
		break;
	case 5:
		remolque->cube.transform.rotate(-90, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 13, y, z + 4);
		break;
	case 6:
		remolque->cube.transform.rotate(0, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x - 4, y, z + 14);
		break;
	case 7:
		remolque->cube.transform.rotate(90, vec3(0, 1, 0));
		remolque->SetTransform(remolque->cube.transform.M);
		remolque->SetPos(x + 8, y, z + 4);
		break;
	default:
		break;
	}
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro Scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	// Draw the world floor
	Plane p(0, 1, 0, 0);
	p.color = cFloor;
	p.Render();

	// Check debug mode
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
	{
		App->debug = !App->debug;
	}
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN)
	{
		App->player->map = !App->player->map;
	}

	// Render all things
	p2List_item<Cube>* buildingItem = buildings.getFirst();
	while (buildingItem)
	{
		buildingItem->data.Render();
		buildingItem = buildingItem->next;
	}
	if (remolque != nullptr)
	{
		remolque->Render();
	}
	PhysBody3D* auxBody;
	for (int i = 0; i < deliverySensor.Count(); i++)
	{
		auxBody = *deliverySensor.At(i);
		if (auxBody != nullptr)
		{
			auxBody->cube.Render();
		}
	}
	tree.Render();
	leaf.Render();
	tree1.Render();
	leaf1.Render();
	tree2.Render();
	leaf2.Render();
	tree3.Render();
	leaf3.Render();
	tree4.Render();
	leaf4.Render();
	tree5.Render();
	leaf5.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (((body1 == remolque && body2 == App->player->vehicle) || (body2 == remolque && body1 == App->player->vehicle)) && isJoint == false)
	{
		App->player->GenerateDeliveryPoint();
		btTransform frameInA, frameInB;
		frameInA = btTransform::getIdentity();
		frameInA.setOrigin(btVector3(btScalar(App->player->vehicle->info.bridge_offset.x), btScalar(App->player->vehicle->info.bridge_offset.y + 1), btScalar(App->player->vehicle->info.bridge_offset.z - 1.5)));
		frameInB = btTransform::getIdentity();
		frameInB.setOrigin(btVector3(btScalar(remolque->info.chassis_offset.x), btScalar(remolque->info.chassis_offset.y), btScalar(remolque->info.chassis_offset.z + 2)));

		cs = new btGeneric6DofConstraint(*App->player->vehicle->GetBody(), *remolque->GetBody(), frameInA, frameInB, false);

		App->audio->PlayFx(1, App->player->lockFx, 0);

		cs->setDbgDrawSize(2.0f);
		App->physics->world->addConstraint(cs);
		App->physics->constraints.add(cs);
		isJoint = true;
	}
	PhysBody3D* auxBody;
	for (int i = 0; i < deliverySensor.Count(); i++)
	{
		auxBody = *deliverySensor.At(i);
		if ((body1 == auxBody && auxBody->isDeliver == true && body2 == App->player->vehicle) || (body2 == auxBody && auxBody->isDeliver == true && body1 == App->player->vehicle))
		{
			if (cs != nullptr)
			{
				for (p2List_item<btTypedConstraint*>* item = App->physics->constraints.getFirst(); item; item = item->next)
				{
					App->physics->world->removeConstraint(item->data);
					delete item->data;
				}
				App->physics->constraints.clear();
				
				App->audio->PlayFx(1, App->player->deliveryFx, 0);

				isJoint = false;
				App->player->wasDelivered = true;
				App->player->GenerateCollectPoint();
				cs = nullptr;
			}
		}
	}
	if (((body1 == remolque && body2 != App->player->vehicle) || (body2 == remolque && body1 != App->player->vehicle)))
	{
		LooseCondition();
	}
}

void ModuleSceneIntro::CreateBuilding(float x, float y, float z, vec3 size, bool axis, Color color)
{
	Cube cube;
	cube.color = color;
	cube.SetPos(x, y/2.0f, z);
	cube.size = size;
	cube.axis = axis;
	buildings.add(cube);

	App->physics->AddBuilding(cube, 100000);
}

void ModuleSceneIntro::LooseCondition()
{
	if (cs != nullptr)
	{
		for (p2List_item<btTypedConstraint*>* item = App->physics->constraints.getFirst(); item; item = item->next)
		{
			App->physics->world->removeConstraint(item->data);
			delete item->data;
		}
		App->physics->constraints.clear();

		isJoint = false;
		App->player->wasDelivered = false;
		App->player->GenerateCollectPoint();
		App->scene_intro->SetTrailerPos(App->player->playerCollectPoint->getX(), 1.5f, App->player->playerCollectPoint->getZ(), App->player->firstPos);
		App->player->SetInitPos();
		cs = nullptr;
	}
}