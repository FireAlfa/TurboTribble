#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"


Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(input);

	// Camera
	AddModule(camera);
	
	// Editor
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);

}

Application::~Application()
{
	for (int i = modulesList.size() - 1; i >= 0; --i)
	{
		delete modulesList[i];
		modulesList[i] = nullptr;
	}

	modulesList.clear();
}


bool Application::Init()
{
	bool ret = true;

	int i = 0;

	// Call Init() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == true; i++)
	{
		ret = modulesList[i]->Init();
	}


	// Call Start() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == true; ++i)
	{
		ret = modulesList[i]->Start();
	}


	return ret;
}


// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();


	// Call PreUpdate() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->PreUpdate(dt);
	}
	// Call Update() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->Update(dt);
	}
	// Call PostUpdate() in all modules
	for (unsigned int i = 0; i < modulesList.size() && ret == UPDATE_CONTINUE; ++i)
	{
		ret = modulesList[i]->PostUpdate(dt);
	}

	SDL_GL_SwapWindow(window->window);

	FinishUpdate();


	return ret;
}


// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)msTimer.Read() / 1000.0f;
	msTimer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (maxFPS == 0 || renderer3D->vSync || maxFPS > screenRefresh)
	{
		Uint32 last_frame_ms = msTimer.Read();
		float wait_time = (1000.f / (float)screenRefresh) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
	else if (maxFPS > 0)
	{
		Uint32 last_frame_ms = msTimer.Read();
		float wait_time = (1000.f / (float)maxFPS) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}

}



bool Application::CleanUp()
{
	bool ret = true;

	// Call CleanUp() in all modules
	for (int i = modulesList.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = modulesList[i]->CleanUp();
	}
	return ret;
}


void Application::AddModule(Module* mod)
{
	modulesList.push_back(mod);
}