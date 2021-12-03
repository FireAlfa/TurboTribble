#include "ModuleCamera3D.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "GameObject.h"

#include "Globals.h"



ModuleCamera3D::ModuleCamera3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{

	x = float3(1.0f, 0.0f, 0.0f);
	y = float3(0.0f, 1.0f, 0.0f);
	z = float3(0.0f, 0.0f, 1.0f);

	position = float3(0.0f, 5.0f, -15.0f);
	reference = float3(0.0f, 0.0f, 0.0f);
	
	CalculateViewMatrix();

	sceneHovered = false;
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	TTLOG("+++++ Loading Camera Module +++++\n");

	LookAt(float3::zero);

	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	TTLOG("+++++ Quitting Camera Module +++++\n");

	return true;
}

// -----------------------------------------------------------------
UpdateStatus ModuleCamera3D::Update(float dt)
{
	float3 newPos(0, 0, 0);
	float speed = cameraSpeed * dt;

	if (sceneHovered)
	{
		// ---- Keyboard Camera Control -----

	// Hold shift to move faster
		if (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT)
			speed = speed * 2.5 * dt;

		// Arrow Movement
		if (app->input->GetKey(SDL_SCANCODE_UP) == KeyState::KEY_REPEAT) newPos += z * speed;
		if (app->input->GetKey(SDL_SCANCODE_DOWN) == KeyState::KEY_REPEAT) newPos -= z * speed;
		if (app->input->GetKey(SDL_SCANCODE_LEFT) == KeyState::KEY_REPEAT) newPos += x * speed;
		if (app->input->GetKey(SDL_SCANCODE_RIGHT) == KeyState::KEY_REPEAT) newPos -= x * speed;

		// Flythrough mode
		if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			if (app->input->GetKey(SDL_SCANCODE_W) == KeyState::KEY_REPEAT) newPos += z * speed;
			if (app->input->GetKey(SDL_SCANCODE_S) == KeyState::KEY_REPEAT) newPos -= z * speed;
			if (app->input->GetKey(SDL_SCANCODE_A) == KeyState::KEY_REPEAT) newPos += x * speed;
			if (app->input->GetKey(SDL_SCANCODE_D) == KeyState::KEY_REPEAT) newPos -= x * speed;

			if (app->input->GetKey(SDL_SCANCODE_E) == KeyState::KEY_REPEAT) newPos.y -= speed;
			if (app->input->GetKey(SDL_SCANCODE_Q) == KeyState::KEY_REPEAT) newPos.y += speed;
		}

		// Focus on Game Object
		if (app->input->GetKey(SDL_SCANCODE_F) == KeyState::KEY_DOWN)
		{
			FrameSelected();
		}
		// ----------------------------------


		// -----  Mouse Camera Control -----

		//Zoom in and out using Scrool Wheel
		if (app->input->GetMouseZ() < 0)
		{
			newPos -= z * speed * 4;
		}
		if (app->input->GetMouseZ() > 0)
		{
			newPos += z * speed * 4;
		}

		// Look around the camera position holding Right Click
		bool hasRotated = false;
		if (app->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::KEY_REPEAT)
		{
			int dx = -app->input->GetMouseXMotion();
			int dy = -app->input->GetMouseYMotion();

			if (dx != 0)
			{
				const float newDeltaX = (float)dx * cameraSensitivity;
				float deltaX = newDeltaX + 0.95f * (lastDeltaX - newDeltaX); // lerp for smooth rotation acceleration to avoid jittering
				lastDeltaX = deltaX;
				Quat rotateY = Quat::RotateY(y.y >= 0.f ? deltaX * .1f : -deltaX * .1f);
				y = rotateY * y;
				z = rotateY * z;
				CalculateViewMatrix();
				hasRotated = true;
			}

			if (dy != 0)
			{
				const float newDeltaY = (float)dy * cameraSensitivity;
				float deltaY = newDeltaY + 0.95f * (lastDeltaY - newDeltaY); // lerp for smooth rotation acceleration to avoid jittering
				lastDeltaY = deltaY;
				Quat rotateX = Quat::RotateAxisAngle(x, -deltaY * .1f);
				y = rotateX * y;
				z = rotateX * z;
				CalculateViewMatrix();
				hasRotated = true;
			}
		}

		// Orbit on GameObject
		if (app->input->GetKey(SDL_SCANCODE_LALT) == KeyState::KEY_REPEAT)
		{
			int dx = -app->input->GetMouseXMotion();
			int dy = -app->input->GetMouseYMotion();

			if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
				if (app->editor->gameobjectSelected != nullptr)
				{
					const float newDeltaX = (float)dx * cameraSensitivity;
					const float newDeltaY = (float)dy * cameraSensitivity;

					reference = app->editor->gameobjectSelected->transform->GetPosition();
					Quat orbitMat = Quat::RotateY(newDeltaX * .1f);

					if (abs(y.y) < 0.3f) // Avoid gimball lock on up & down apex
					{
						if (position.y > reference.y && newDeltaY < 0.f)
							orbitMat = orbitMat * math::Quat::RotateAxisAngle(x, newDeltaY * .1f);
						if (position.y < reference.y && newDeltaY > 0.f)
							orbitMat = orbitMat * math::Quat::RotateAxisAngle(x, newDeltaY * .1f);
					}
					else
					{
						orbitMat = orbitMat * math::Quat::RotateAxisAngle(x, newDeltaY * .1f);
					}

					position = orbitMat * (position - reference) + reference;

					CalculateViewMatrix();
					LookAt(reference);
				}
			}
		}

		!hasRotated ? lastDeltaX = lastDeltaY = 0.f : 0.f;
		// ---------------------------------
	}

	// Update position
	position += newPos;

	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleCamera3D::FrameSelected()
{
	if (app->editor->gameobjectSelected != nullptr)
	{
		if (ComponentMesh* mesh = app->editor->gameobjectSelected->GetComponent<ComponentMesh>())
		{
			const float3 meshCenter = mesh->GetCenterPointInWorldCoords();
			LookAt(meshCenter);
			const float meshRadius = mesh->GetSphereRadius();
			const float currentDistance = meshCenter.Distance(position);
			const float desiredDistance = (meshRadius * 2) / atan(cameraFrustum.horizontalFov);
			position = position + z * (currentDistance - desiredDistance);
		}
		else
		{
			LookAt(app->editor->gameobjectSelected->transform->GetPosition());
		}
	}
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt(const float3& point)
{		
	reference = point;

	z = (reference - position).Normalized();
	x = float3(0.0f, 1.0f, 0.0f).Cross(z).Normalized();
	y = z.Cross(x);

	CalculateViewMatrix();
}



// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	if (projectionIsDirty)
		RecalculateProjection();

	cameraFrustum.pos = position;
	cameraFrustum.front = z.Normalized();
	cameraFrustum.up = y.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	x = y.Cross(z);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ModuleCamera3D::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = (verticalFOV * 3.141592 / 2) / 180.f;
	cameraFrustum.horizontalFov = 2.f * atanf(tanf(cameraFrustum.verticalFov * 0.5f) * aspectRatio);
}

void ModuleCamera3D::OnGui()
{
	if (ImGui::CollapsingHeader("Editor Camera"))
	{
		if (ImGui::DragFloat("Vertical fov", &verticalFOV))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Near plane distance", &nearPlaneDistance))
		{
			projectionIsDirty = true;
		}
		if (ImGui::DragFloat("Far plane distance", &farPlaneDistance))
		{
			projectionIsDirty = true;
		}
	}
}

void ModuleCamera3D::OnSave(JSONWriter& writer) const
{
	writer.String("camera");	
	writer.StartObject();
	SAVE_JSON_FLOAT(verticalFOV)
	SAVE_JSON_FLOAT(nearPlaneDistance)
	SAVE_JSON_FLOAT(farPlaneDistance)
	SAVE_JSON_FLOAT(cameraSpeed)
	SAVE_JSON_FLOAT(cameraSensitivity)
	writer.EndObject();
}

void ModuleCamera3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("camera"))
	{
		const auto& config = reader["camera"];
		LOAD_JSON_FLOAT(verticalFOV);
		LOAD_JSON_FLOAT(nearPlaneDistance);
		LOAD_JSON_FLOAT(farPlaneDistance);
		LOAD_JSON_FLOAT(cameraSpeed);
		LOAD_JSON_FLOAT(cameraSensitivity);
	}
	RecalculateProjection();
}