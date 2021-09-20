#include "Application.h"

Application::Application()
{
	PERF_START(pTimer);

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	
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
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}

	list_modules.clear();
}

void Application::AddModule(Module* mod)
{
	list_modules.add(mod);
}

bool Application::Init()
{
	PERF_START(pTimer);

	bool ret = true;

	// Call Init() in all modules
	p2List_item<Module*>* item = list_modules.getFirst();

	while(item != NULL && ret == true)
	{
		ret = item->data->Init();
		item = item->next;
	}

	frameRateCap = 50;
	screenTicksCap = 1000 / frameRateCap;


	PERF_PEEK(pTimer);


	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = list_modules.getFirst();

	fpsMSeconds = SDL_GetTicks();

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}

	

	return ret;
}


// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	p2List_item<Module*>* item = list_modules.getFirst();

	dt = dtTimer.ReadSec();
	dtTimer.Start();

	while (item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PreUpdate(dt);
		item = item->next;
	}

	item = list_modules.getFirst();

	while (item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}

	item = list_modules.getFirst();

	while (item != NULL && ret == UPDATE_CONTINUE)
	{
		ret = item->data->PostUpdate(dt);
		item = item->next;
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
	p2List_item<Module*>* item = list_modules.getLast();

	while(item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
	}
	return ret;
}