#ifndef __MODULE_SCENE_INTRO_H__
#define __MODULE_SCENE_INTRO_H__

#include "Module.h"

#include "p2DynArray.h"



class ModuleSceneIntro : public Module
{
public:

	// Constructor
	ModuleSceneIntro(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleSceneIntro();

	// Load Scene
	bool Start();
	// Update: draw background Plane
	UpdateStatus Update(float dt);
	// Unload Scene
	bool CleanUp();
};

#endif // !__MODULE_SCENE_INTRO_H__