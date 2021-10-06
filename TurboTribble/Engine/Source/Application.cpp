#include "Application.h"

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"



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

	// The order in which we do AddModule is very important!
	// Modules will Init() Start() and Update() in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes
	AddModule(scene_intro);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);
}
// Application Destructor
Application::~Application()
{
	p2List_item<Module*>* item = modulesList.getLast();

	while(item != NULL)
	{
		delete item->data;
		item = item->prev;
	}
}


// Initialization of the Application, this calls all Modules' Init() and Start()
bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	p2List_item<Module*>* item = modulesList.getFirst();

	while(item != NULL && ret == true)
	{
		ret = item->data->Init();
		item = item->next;
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	item = modulesList.getFirst();

	while(item != NULL && ret == true)
	{
		ret = item->data->Start();
		item = item->next;
	}
	
	msTimer.Start();
	return ret;
}
// Call PreUpdate(), Update() and PostUpdate() of all Modules
UpdateStatus Application::Update()
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;
	PrepareUpdate();

	p2List_item<Module*>* item = modulesList.getFirst();

	while (item != NULL && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = item->data->PreUpdate(dt);
		item = item->next;
	}

	item = modulesList.getFirst();

	while (item != NULL && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = item->data->Update(dt);
		item = item->next;
	}

	item = modulesList.getFirst();

	while (item != NULL && ret == UpdateStatus::UPDATE_CONTINUE)
	{
		ret = item->data->PostUpdate(dt);
		item = item->next;
	}

	FinishUpdate();
	return ret;
}
// Call CleanUp() of all Modules
bool Application::CleanUp()
{
	bool ret = true;
	p2List_item<Module*>* item = modulesList.getLast();

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		item = item->prev;
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
}


// Adds a Module to the Module vector
void Application::AddModule(Module* mod)
{
	modulesList.add(mod);
}