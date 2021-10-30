#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleMeshLoader.h"



// Application Constructor
Application::Application()
{
	// Constructor called for every Module used
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	loader = new ModuleMeshLoader(this);

	// The order in which we do AddModule is very important!
	// Modules will Init() Start() and Update() in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	AddModule(loader);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);
}
// Application Destructor
Application::~Application()
{
	for (int i = modulesList.size() - 1; i >= 0; --i)
	{
		delete modulesList[i];
		modulesList[i] = nullptr;
	}

	modulesList.clear();
}


// Initialization of the Application, this calls all Modules' Init() and Start()
bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == true; i++)
	{
		ret = modulesList[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	TTLOG("++++++++ Application Start ++++++++");
	for (unsigned int i = 0; i < modulesList.size() && ret == true; ++i)
	{
		ret = modulesList[i]->Start();
	}
	
	msTimer.Start();
	return ret;
}
// Call PreUpdate(), Update() and PostUpdate() of all Modules
UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;
	PrepareUpdate();

	for (unsigned int i = 0; i < modulesList.size() && ret == UpdateStatus::UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->PreUpdate(dt);
	}
	for (unsigned int i = 0; i < modulesList.size() && ret == UpdateStatus::UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->Update(dt);
	}
	for (unsigned int i = 0; i < modulesList.size() && ret == UpdateStatus::UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}
// Call CleanUp() of all Modules
bool Application::CleanUp()
{
	bool ret = true;

	for (int i = modulesList.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = modulesList[i]->CleanUp();
	}

	return ret;
}


// Called each loop before calling the Modules' Update() methods
void Application::PrepareUpdate()
{
	dt = (float)msTimer.Read() / 1000.0f;
	msTimer.Start();
}
// Called each loop after calling the Modules' Update() methods
void Application::FinishUpdate()
{
	float k = (1000 / maxFPS) - dt;
	if (k > 0)
	{
		SDL_Delay((Uint32)k);
	}
}


// Adds a Module to the Module vector
void Application::AddModule(Module* mod)
{
	modulesList.push_back(mod);
}