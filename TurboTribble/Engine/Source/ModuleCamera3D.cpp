#include "ModuleCamera3D.h"
#include "Application.h"

#include "ModuleInput.h"



// Constructor
ModuleCamera3D::ModuleCamera3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	CalculateViewMatrix();

	x = vec3(1.0f, 0.0f, 0.0f);
	y = vec3(0.0f, 1.0f, 0.0f);
	z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(0.0f, 0.0f, 5.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
}
// Destructor
ModuleCamera3D::~ModuleCamera3D()
{
}

// Start
bool ModuleCamera3D::Start()
{
	TTLOG("+++++ Setting up the camera +++++");
	bool ret = true;

	backgroundColor = colorBlack;

	return ret;
}
// Camera that moves with keys and mouse
UpdateStatus ModuleCamera3D::Update(float dt)
{
	// ---- Keyboard Camera Control -----

	vec3 newPos(0, 0, 0);
	float speed = 5.0f * dt;

	// Hold shift to move faster
	if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT)
		speed = 12.0f * dt;

	// Arrow Movement
	if (app->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT) newPos -= z * speed;
	if (app->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT) newPos += z * speed;
	if (app->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT) newPos -= x * speed;
	if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT) newPos += x * speed;

	// Flythrough mode
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT) newPos -= z * speed;
		if (app->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT) newPos += z * speed;
		if (app->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT) newPos -= x * speed;
		if (app->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT) newPos += x * speed;

		if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT) newPos.y += speed;
		if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT) newPos.y -= speed;
	}

	// Focus on GameObject
	if (app->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
	{
		LookAt({ 0,0,0 });
	}
	// ----------------------------------


	// -----  Mouse Camera Control -----

	// Zoom in and out using Scrool Wheel
	if (app->input->GetMouseZ() < 0)
	{
		newPos += z * speed * 4;
	}
	if (app->input->GetMouseZ() > 0)
	{
		newPos -= z * speed * 4;
	}

	// Look around the camera position holding Right Click
	if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
	{
		int dx = -app->input->GetMouseXMotion();
		int dy = -app->input->GetMouseYMotion();

		float sensitivity = 0.25f;

		position -= reference;

		if (dx != 0)
		{
			float deltaX = (float)dx * sensitivity;

			x = rotate(x, deltaX, vec3(0.0f, 1.0f, 0.0f));
			y = rotate(y, deltaX, vec3(0.0f, 1.0f, 0.0f));
			z = rotate(z, deltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float deltaY = (float)dy * sensitivity;

			y = rotate(y, deltaY, x);
			z = rotate(z, deltaY, x);

			if (y.y < 0.0f)
			{
				z = vec3(0.0f, z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				y = cross(z, x);
			}
		}

		position = reference + z * length(position);
	}
	// ---------------------------------



	// Update position
	position += newPos;
	reference += newPos;
	// Recalculate matrix after calculations
	CalculateViewMatrix();

	return UpdateStatus::UPDATE_CONTINUE;
}
// CleanUp
bool ModuleCamera3D::CleanUp()
{
	TTLOG("+++++ Cleaning camera +++++");

	return true;
}



// Look
void ModuleCamera3D::Look(const vec3 &position, const vec3 &reference, bool rotateAroundReference)
{
	this->position = position;
	this->reference = reference;

	z = normalize(position - reference);
	x = normalize(cross(vec3(0.0f, 1.0f, 0.0f), z));
	y = cross(z, x);

	if(!rotateAroundReference)
	{
		this->reference = this->position;
		this->position += z * 0.05f;
	}

	CalculateViewMatrix();
}
// Look at a given spot
void ModuleCamera3D::LookAt(const vec3 &spot)
{
	reference = spot;

	z = normalize(position - reference);
	x = normalize(cross(vec3(0.0f, 1.0f, 0.0f), z));
	y = cross(z, x);

	CalculateViewMatrix();
}
// Apply movement to the camera position and reference
void ModuleCamera3D::Move(const vec3 &movement)
{
	position += movement;
	reference += movement;

	CalculateViewMatrix();
}


// View Matrix Getter
float* ModuleCamera3D::GetViewMatrix()
{
	return &viewMatrix;
}
// Calculate View Matrix
void ModuleCamera3D::CalculateViewMatrix()
{
	viewMatrix = mat4x4(x.x, y.x, z.x, 0.0f, x.y, y.y, z.y, 0.0f, x.z, y.z, z.z, 0.0f, -dot(x, position), -dot(y, position), -dot(z, position), 1.0f);
	viewMatrixInverse = inverse(viewMatrix);
}
