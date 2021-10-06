#include "ModuleSceneIntro.h"
#include "Application.h"

#include "Primitive.h"
#include "ModuleCamera3D.h"



// Constructor
ModuleSceneIntro::ModuleSceneIntro(Application* app, bool startEnabled) : Module(app, startEnabled)
{
}
// Destrucotr
ModuleSceneIntro::~ModuleSceneIntro()
{
}


// Load Scene
bool ModuleSceneIntro::Start()
{
	LOG("Loading Scene");
	bool ret = true;

	// Set the initial camera position
	app->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	app->camera->LookAt(vec3(0, 0, 0));
	return ret;
}
// Update: draw background Plane
UpdateStatus ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UpdateStatus::UPDATE_CONTINUE;
}
// Unload Scene
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Scene");

	return true;
}