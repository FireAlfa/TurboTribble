#pragma once

#include <vector>

#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL_opengl.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;

private:

	PerfTimer pTimer;
	uint64 frameCount = 0;
	Timer startupTime;
	Timer dtTimer;
	float dt = 0.0f;
	float secondsSinceStartup;
	float fpsMSeconds;
	float fpsCounter;
	float fpsPreUpdate;
	float frameRateCap;
	float screenTicksCap;

	std::vector<Module*> list_modules;

public:

	float fps = 0.0f;
	bool debug = false;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};