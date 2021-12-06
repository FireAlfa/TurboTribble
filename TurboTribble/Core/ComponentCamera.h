#ifndef __COMPONENT_CAMERA_H__
#define __COMPONENT_CAMERA_H__

#include "Component.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"



class ComponentCamera : public Component
{
public:

	// Constructor
	ComponentCamera(GameObject* parent);
	// Destructor
	~ComponentCamera() {}

	// Update
	bool PreUpdate(float dt);
	bool Update(float dt) override;


	// Look at a given spot
	void LookAt(const float3& point);

	// Recalculate Projection
	void RecalculateProjection();

	// Calculate View Matrix
	void CalculateViewMatrix();


	void OnGui() override;

	// ----- Getters -----

	Frustum GetCameraFrustum()
	{
		return cameraFrustum;
	}

public:

	Frustum cameraFrustum;
	float aspectRatio;

private:

	// ----- Camera Variables -----

	float3 right, up, front, reference;
	float4x4 viewMatrix;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 5000.f;
	float cameraSensitivity = 0.25f;
	float cameraSpeed = 5.f;
	bool projectionIsDirty = false;
	float lastDeltaX = 0.f, lastDeltaY = 0.f;
	// ----------------------------

};

#endif // !__COMPONENT_CAMERA_H__