#include "ComponentCamera.h"

#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleViewportFrameBuffer.h"

#include "Globals.h"

#include <math.h>
#include "ImGui/imgui.h"
#include "SDL/include/SDL_opengl.h"
#include "Geometry/LineSegment.h"



ComponentCamera::ComponentCamera(GameObject* parent) : Component(parent)
{
	aspectRatio = 16 / 9;

	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = verticalFOV;
	cameraFrustum.horizontalFov = atan(aspectRatio * tan(cameraFrustum.verticalFov / 2)) * 2;
	cameraFrustum.front = owner->transform->Front();
	cameraFrustum.up = owner->transform->Up();
	cameraFrustum.pos = owner->transform->GetPosition();
	viewMatrix = cameraFrustum.ViewMatrix();

	LookAt(float3::zero);
}

bool ComponentCamera::PreUpdate(float dt)
{
	app->gameViewportBuffer->PreUpdate(dt);

	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(viewMatrix.Transposed().ptr());

	return true;
}

bool ComponentCamera::Update(float dt)
{
	cameraFrustum.pos = owner->transform->GetPosition();
	cameraFrustum.front = owner->transform->Front();
	cameraFrustum.up = owner->transform->Up();
	viewMatrix = cameraFrustum.ViewMatrix();

	if (projectionIsDirty)
		RecalculateProjection();

	return true;
}

void ComponentCamera::Draw()
{
	// Debug Draw
	glBindTexture(GL_TEXTURE_2D, 0);
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(cameraFrustum.Edge(i).a.x, cameraFrustum.Edge(i).a.y, cameraFrustum.Edge(i).a.z);
		glVertex3f(cameraFrustum.Edge(i).b.x, cameraFrustum.Edge(i).b.y, cameraFrustum.Edge(i).b.z);
	}

	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void ComponentCamera::LookAt(const float3& point)
{
	reference = point;

	front = (reference - cameraFrustum.pos).Normalized();
	right = float3(0.0f, 1.0f, 0.0f).Cross(front).Normalized();
	up = front.Cross(right);

	CalculateViewMatrix();
}

void ComponentCamera::CalculateViewMatrix()
{
	if (projectionIsDirty)
		RecalculateProjection();

	cameraFrustum.pos = owner->transform->GetPosition();
	cameraFrustum.front = front.Normalized();
	cameraFrustum.up = up.Normalized();
	float3::Orthonormalize(cameraFrustum.front, cameraFrustum.up);
	right = up.Cross(front);
	viewMatrix = cameraFrustum.ViewMatrix();
}

void ComponentCamera::RecalculateProjection()
{
	cameraFrustum.type = FrustumType::PerspectiveFrustum;
	cameraFrustum.nearPlaneDistance = nearPlaneDistance;
	cameraFrustum.farPlaneDistance = farPlaneDistance;
	cameraFrustum.verticalFov = verticalFOV;
	cameraFrustum.horizontalFov = atan(aspectRatio * tan(cameraFrustum.verticalFov / 2)) * 2;
}

void ComponentCamera::OnGui()
{
	if (ImGui::CollapsingHeader("Camera"))
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