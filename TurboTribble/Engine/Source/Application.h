#pragma once

#include <vector>
#include <list>

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "SDL/include/SDL_opengl.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;

	std::vector<Module*> modulesList;

private:

	Timer msTimer;
	float dt;

public:

	int maxFPS;
	int screenRefresh;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDeltaTime() const { return dt; }
	float GetFrameRate() const { return 1.0f / dt; }

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	ImGuiIO io;
};

//exter Application* app;