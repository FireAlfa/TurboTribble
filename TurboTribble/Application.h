#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
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

	p2List<Module*> list_modules;

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