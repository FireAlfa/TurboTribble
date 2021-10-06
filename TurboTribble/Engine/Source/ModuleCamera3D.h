#ifndef __MODULE_CAMERA_H__
#define __MODULE_CAMERA_H__

#include "Module.h"

#include "glmath.h"



class ModuleCamera3D : public Module
{
public:

	// Constructor
	ModuleCamera3D(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleCamera3D();

	// Start
	bool Start();
	// 
	UpdateStatus Update(float dt);
	// CleanUp
	bool CleanUp();


	// Look
	void Look(const vec3& position, const vec3& reference, bool rotateAroundReference = false);
	// Look at a given spot
	void LookAt(const vec3& spot);
	// Apply movement to the camera position and reference
	void Move(const vec3& movement);

	// View Matrix Getter
	float* GetViewMatrix();

private:

	// Calculate View Matrix
	void CalculateViewMatrix();

public:

	// Camera public variables
	vec3 x, y, z, position, reference;

private:

	// Camera private variables
	mat4x4 viewMatrix, viewMatrixInverse;
};

#endif // !__MODULE_CAMERA_H__