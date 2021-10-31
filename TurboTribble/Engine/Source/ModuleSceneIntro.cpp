#include "ModuleSceneIntro.h"
#include "Application.h"

#include "Primitive.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleMeshLoader.h"



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
	TTLOG("Loading Scene");
	bool ret = true;

	// Set the initial camera position
	app->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	app->camera->LookAt(vec3(0, 0, 0));

	app->loader->LoadMesh("Assets/BakerHouse.fbx");
	return ret;
}
// Update
UpdateStatus ModuleSceneIntro::Update(float dt)
{

	return UpdateStatus::UPDATE_CONTINUE;
}
// Unload Scene
bool ModuleSceneIntro::CleanUp()
{
	TTLOG("Unloading Scene");

	return true;
}


// Draw everything on screen, called by ModuleRenderer
void ModuleSceneIntro::Draw()
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
}

GameObject* CreateGameObject(const char* name)
{

	GameObject* gameObject = new GameObject(name);
	
	return gameObject;
}