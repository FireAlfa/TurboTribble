#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleMeshLoader : public Module
{
public:
	ModuleMeshLoader(Application* app, bool start_enabled = true);
	~ModuleMeshLoader();

	bool Init();
	UpdateStatus PreUpdate(float dt);
	UpdateStatus PostUpdate(float dt);

	bool CleanUp();
public:

};