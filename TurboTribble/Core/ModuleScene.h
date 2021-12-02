#ifndef __MODULE_SCENE_H__
#define __MODULE_SCENE_H__

#include "Module.h"
#include "ModuleImport.h"
#include "GameObject.h"

#include "Globals.h"



class ModuleScene : public Module
{
public:

	// Constructor
	ModuleScene(Application* app, bool startEnabled = true);

	// Init Scene
	bool Init() override;
	// Load Scene
	bool Start() override;
	// Called each iteration
	UpdateStatus Update(float dt) override;
	// Called before quitting
	bool CleanUp() override;

	// ----- Game Object Creators -----
	
	GameObject* CreateGameObject(GameObject* parent = nullptr);
	GameObject* CreateGameObject(const std::string name, GameObject* parent = nullptr);
	// --------------------------------

public:

	// Root
	GameObject* root;
};

#endif // !__MODULE_SCENE_H__