#ifndef __MODULE_CAMERA_3D_H__
#define __MODULE_CAMERA_3D_H__

#include "Module.h"

#include "Globals.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"



class ModuleCamera3D : public Module
{
public:

	// Constructor
	ModuleCamera3D(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleCamera3D();

	// Start
	bool Start() override;
	// Update
	UpdateStatus Update(float dt)override;
	void FrameSelected();
	// CleanUp
	bool CleanUp() override;


	// Look at a given spot
	void LookAt(const float3& point);

	void OnGui() override;
	void OnSave(JSONWriter& writer) const override;
	void OnLoad(const JSONReader& reader) override;

	// Recalculate Projection
	void RecalculateProjection();

	// Calculate View Matrix
	void CalculateViewMatrix();

public:

	// ----- Camera Variables -----

	float aspectRatio = 1.f;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float3 position;
	// ----------------------------

private:

	// ----- Camera Variables -----
	
	float3 x, y, z, reference;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 5000.f;
	float cameraSensitivity = 0.25f;
	float cameraSpeed = 5.f;
	bool projectionIsDirty = false;
	float lastDeltaX = 0.f, lastDeltaY = 0.f;
	// ----------------------------

};

#endif // !__MODULE_CAMERA_3D_H__