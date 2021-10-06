#ifndef __MODULE_RENDERER_3D_H__
#define __MODULE_RENDERER_3D_H__

#include "Module.h"

#include "glmath.h"
#include "Light.h"
#include "SDL/include/SDL.h"

#define MAX_LIGHTS 8



class ModuleRenderer3D : public Module
{
public:

	// Constructor
	ModuleRenderer3D(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleRenderer3D();

	// Initialize the renderer
	bool Init();
	// Clear the buffer
	UpdateStatus PreUpdate(float dt);
	// Present the buffer to screen
	UpdateStatus PostUpdate(float dt);
	// Called before quitting, destroys the renderer
	bool CleanUp();


	// Control what happens when the window is resized
	void OnResize(int width, int height);

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext glContext;
	mat3x3 normalMatrix;
	mat4x4 modelMatrix, viewMatrix, projectionMatrix;
};

#endif // !__MODULE_RENDERER_3D_H__