#ifndef __MODULE_EDITOR_CAMERA_H__
#define __MODULE_EDITOR_CAMERA_H__

#include "Module.h"

#include "Globals.h"

#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"
#include "Geometry/LineSegment.h"


class GameObject;

class ModuleEditorCamera : public Module
{
public:

	// Constructor
	ModuleEditorCamera(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleEditorCamera();

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

private:

	void MousePicking();
	GameObject* RayHitResult(const LineSegment& rayPicking);

public:

	// ----- Camera Variables -----

	float aspectRatio = 1.f;
	Frustum cameraFrustum;
	float4x4 viewMatrix;
	float3 position;
	bool sceneHovered;
	// ----------------------------


	// ----- Camera Flags -----
	
	bool cullingActivated = false;
	bool drawMousePickRayCast = true;
	// ------------------------


	// ----- Mouse Picking -----

	LineSegment rayPicking;
	// -------------------------

private:

	// ----- Camera Variables -----
	
	float3 right, up, front, reference;
	float verticalFOV = 60.f;
	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 5000.f;
	float cameraSensitivity = 0.25f;
	float cameraSpeed = 5.f;
	bool projectionIsDirty = false;
	float lastDeltaX = 0.f, lastDeltaY = 0.f;
	// ----------------------------

};

#endif // !__MODULE_EDITOR_CAMERA_H__