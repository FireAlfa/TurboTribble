#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleSceneIntro.h"
#include "Bullet/include/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"
#include "time.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	//Load all engine sound effects
	gearFx = App->audio->LoadFx("Assets/FX/gear_shift.wav");
	engineFx = App->audio->LoadFx("Assets/FX/truck_engine.wav");
	engineOnFx = App->audio->LoadFx("Assets/FX/truck_on.wav");
	engineOffFx = App->audio->LoadFx("Assets/FX/truck_off.wav");
	engineAccelerationFx = App->audio->LoadFx("Assets/FX/truck_acceleration.wav");
	lockFx = App->audio->LoadFx("Assets/FX/truck_lock.wav");
	deliveryFx = App->audio->LoadFx("Assets/FX/delivery.wav");
	reverseFx = App->audio->LoadFx("Assets/FX/truck_reverse.wav");
	App->audio->PlayMusic("Assets/FX/radio.wav");

	//Play stop engine sound
	App->audio->PlayFx(1, engineOnFx);
	App->audio->PlayFx(0, engineFx, -1);

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.chassis_size.Set(2.5f, 3, 2.f);
	car.chassis_offset.Set(0, 1.5, 0);
	car.bridge_size.Set(2.5f, 0.7f, 3.f);
	car.bridge_offset.Set(0.0f, 0.37f, -2.5f);

	car.mass = 300.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Don't change anything below this line ------------------
	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;
	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius - 3);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius - 3);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	car.color = cTruck;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0.0f - 2.0f, 1.5f, 14.0f);
	vehicle->collision_listeners.add(this);
	vehicle->GetBody()->setUserPointer(vehicle);

	GenerateCollectPoint();
	App->scene_intro->CreateTrailer(playerCollectPoint->getX(), 1.5f, playerCollectPoint->getZ(), firstPos);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	if (map == false)
	{
		turn = acceleration = 0;
		brake = 50.0f;
		if (engine == true)
		{
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			{
				switch (gearState)
				{
				case ModulePlayer::PARKING:
					break;
				case ModulePlayer::DRIVE:
					if (Mix_Paused(3) == 0)
					{
						App->audio->PauseFx(3);
					}
					if (Mix_Paused(2) == 1)
					{
						App->audio->ResumeFx(2);
					}
					if (isAccel != true)
					{
						App->audio->PlayFx(2, engineAccelerationFx, -1);
						isAccel = true;
						isDesaccel = false;
					}
					if (vehicle->GetKmh() < 90)
					{
						acceleration = MAX_ACCELERATION;
					}
					break;
				case ModulePlayer::REVERSE:
					if (vehicle->GetKmh() > -20)
					{
						acceleration = -MAX_ACCELERATION;
					}
					break;
				default:
					break;
				}
			}
			else
			{
				if (Mix_Paused(2) == 0)
				{
					App->audio->PauseFx(2);
				}
				if (Mix_Paused(3) == 1)
				{
					App->audio->ResumeFx(3);
				}
				if (isDesaccel != true && vehicle->GetKmh() > 5)
				{
					isDesaccel = true;
				}
			}

			if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			{
				brake = BRAKE_POWER;
			}

			if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			{
				if (turn < TURN_DEGREES)
					turn += TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			{
				if (turn > -TURN_DEGREES)
					turn -= TURN_DEGREES;
			}

			if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			{
				App->audio->PlayFx(1, gearFx);
				switch (gearState)
				{
				case ModulePlayer::PARKING:
					gearState = DRIVE;
					break;
				case ModulePlayer::DRIVE:
					gearState = REVERSE;
					break;
				case ModulePlayer::REVERSE:
					gearState = PARKING;
					break;
				default:
					break;
				}
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
		{
			if (engine != true)
			{
				App->audio->PlayFx(1, engineOnFx);
				App->audio->ResumeFx(0);
				if (radio == true)
				{
					Mix_ResumeMusic();
				}
				engine = true;
			}
			else
			{
				App->audio->PauseFx(0);
				App->audio->PlayFx(1, engineOffFx);
				Mix_PauseMusic();
				engine = false;
			}
		}

		if (engine == true)
		{
			if (gearState == ModulePlayer::REVERSE)
			{
				App->audio->PlayFx(4, reverseFx, 0);
			} 
			else
			{
				App->audio->PauseFx(4);
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN && engine == true)
		{
			if (radio != true)
			{
				Mix_ResumeMusic();
				radio = true;
			}
			else
			{
				Mix_PauseMusic();
				radio = false;
			}
		}

		vehicle->ApplyEngineForce(acceleration * dt);
		vehicle->Turn(turn);
		vehicle->Brake(brake * dt);
	}

	vehicle->Render();

	char title[80];
	if (engine == true)
	{
		switch (gearState)
		{
		case ModulePlayer::PARKING:
			sprintf_s(title, "%.1f Km/h, Gear: Parking, Engine: On | dt: %f, FPS: %d", vehicle->GetKmh(), dt, (int)App->fps);
			break;
		case ModulePlayer::DRIVE:
			sprintf_s(title, "%.1f Km/h, Gear: Drive, Engine: On | dt: %f, FPS: %d", vehicle->GetKmh(), dt, (int)App->fps);
			break;
		case ModulePlayer::REVERSE:
			sprintf_s(title, "%.1f Km/h, Gear: Reverse, Engine: On | dt: %f, FPS: %d", vehicle->GetKmh(), dt, (int)App->fps);
			break;
		default:
			break;
		}
	}
	else sprintf_s(title, "%.1f Km/h, Gear: None, Engine: Off | dt: %f, FPS: %d", vehicle->GetKmh(), dt, (int)App->fps);
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::GenerateDeliveryPoint()
{
	srand(time(NULL));
	int r = rand() % App->scene_intro->deliveryPoints.Count();

	if (App->scene_intro->deliveryPoints.At(r) == playerCollectPoint)
	{
		if (r == 0)
		{
			playerDeliveryPoint = App->scene_intro->deliveryPoints.At(r + 1);
		}
		else if (r == 7)
		{
			playerDeliveryPoint = App->scene_intro->deliveryPoints.At(r - 1);
		}
		else
		{
			playerDeliveryPoint = App->scene_intro->deliveryPoints.At(r + 1);
		}
	}
	else
	{
		playerDeliveryPoint = App->scene_intro->deliveryPoints.At(r);
	}
	PhysBody3D* auxBody;
	for (int i = 0; i < App->scene_intro->deliveryPoints.Count(); i++)
	{
		if (App->scene_intro->deliveryPoints.At(i) == playerDeliveryPoint)
		{
			auxBody = *App->scene_intro->deliverySensor.At(i);
			auxBody->cube.color = Yellow;
			auxBody->isDeliver = true;
		}
	}
}

void ModulePlayer::GenerateCollectPoint()
{
	srand(time(NULL));
	int r = rand() % App->scene_intro->deliveryPoints.Count();
	firstPos = r;

	PhysBody3D* auxBody;
	for (int i = 0; i < App->scene_intro->deliveryPoints.Count(); i++)
	{
		auxBody = *App->scene_intro->deliverySensor.At(i);
		auxBody->cube.color = Blue;
		auxBody->isDeliver == false;
	}
	
	if (App->scene_intro->deliveryPoints.At(r) == playerDeliveryPoint)
	{
		if (r == 0)
		{
			playerCollectPoint = App->scene_intro->deliveryPoints.At(r + 1);
		}
		else if (r == 7)
		{
			playerCollectPoint = App->scene_intro->deliveryPoints.At(r - 1);
		}
		else
		{
			playerCollectPoint = App->scene_intro->deliveryPoints.At(r + 1);
		}
	}
	else
	{
		playerCollectPoint = App->scene_intro->deliveryPoints.At(r);
	}
	if (wasDelivered == true)
	{
		App->scene_intro->SetTrailerPos(playerCollectPoint->getX(), 1.5f, playerCollectPoint->getZ(), r);
	}
}

void ModulePlayer::SetInitPos()
{
	gearState = PARKING;
	vehicle->GetBody()->setLinearVelocity({ 0,0,0 });
	vehicle->GetBody()->setAngularVelocity({ 0,0,0 });
	vehicle->SetTransform((0, 0, 0, 0));
	vehicle->SetPos(0.0f - 2.0f, 1.5f, 14.0f);
}