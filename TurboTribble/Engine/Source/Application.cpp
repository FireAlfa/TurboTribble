#include "Application.h"

Application::Application()
{
	PERF_START(pTimer);

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	
	// Scenes


	// Camera
	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);


	bool debug = false;


	PERF_PEEK(pTimer);
}

Application::~Application()
{
	for (int i = list_modules.size() - 1; i >= 0; --i)
	{
		delete list_modules[i];
		list_modules[i] = nullptr;
	}

	list_modules.clear();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

bool Application::Init()
{
	PERF_START(pTimer);

	bool ret = true;

	// Call Init() in all modules
	for (unsigned int i = 0; i >= list_modules.size() && ret == true; ++i)
	{
		ret = list_modules[i]->Init();
	}

	//TODO No sé que es lo del perf timer
	frameRateCap = 50;
	screenTicksCap = 1000 / frameRateCap;


	PERF_PEEK(pTimer);


	// After all Init calls we call Start() in all modules
	//LOG("Application Start --------------");
	


	fpsMSeconds = SDL_GetTicks();

	// Call Start() in all modules
	for (unsigned int i = 0; i >= list_modules.size() && ret == true; ++i)
	{
		ret = list_modules[i]->Start();
	}

	

	return ret;
}


// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	dt = dtTimer.ReadSec();
	dtTimer.Start();

	// Call PreUpdate() in all modules
	for (unsigned int i = 0; i >= list_modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}

	// Call Update() in all modules
	for (unsigned int i = 0; i >= list_modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = list_modules[i]->Update(dt);
	}

	// Call PostUpdate() in all modules
	for (unsigned int i = 0; i >= list_modules.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}


// ---------------------------------------------
void Application::PrepareUpdate()
{
	secondsSinceStartup = startupTime.ReadSec();
	fpsPreUpdate = SDL_GetTicks();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	uint32 lastFrameMs = 0;
	uint32 framesOnLastUpdate = 0;

	frameCount++;
	float averageFps = frameCount / secondsSinceStartup;

	++fpsCounter;
	float fpsPostUpdate = SDL_GetTicks() - fpsPreUpdate;

	if (fpsMSeconds < SDL_GetTicks() - 1000)
	{
		fpsMSeconds = SDL_GetTicks();
		fps = fpsCounter;
		fpsCounter = 0;
	}

	if (fpsPostUpdate < screenTicksCap)
	{
		SDL_Delay(screenTicksCap - fpsPostUpdate);
	}

}



bool Application::CleanUp()
{
	bool ret = true;

	for (int i = list_modules.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = list_modules[i]->CleanUp();
	}
	return ret;
}