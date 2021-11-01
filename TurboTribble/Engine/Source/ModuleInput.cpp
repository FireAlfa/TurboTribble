#include "ModuleInput.h"
#include "Application.h"

#include "ModuleRenderer3D.h"
#include "GameObject.h"
#include "ModuleSceneIntro.h"

#include "imgui/imgui_impl_sdl.h"



// Constructor
ModuleInput::ModuleInput(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	// Initialize the Keyboard array
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
}
// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}


// Initialize the input events subsystem
bool ModuleInput::Init()
{
	TTLOG("+++++ Init Module Input +++++");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		TTLOG("##### SDL_EVENTS could not initialize! SDL_Error: %s #####\n", SDL_GetError());
		ret = false;
	}

	return ret;
}
// Called every draw update
UpdateStatus ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KeyState::KEY_IDLE)
				keyboard[i] = KeyState::KEY_DOWN;
			else
				keyboard[i] = KeyState::KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KeyState::KEY_REPEAT || keyboard[i] == KeyState::KEY_DOWN)
				keyboard[i] = KeyState::KEY_UP;
			else
				keyboard[i] = KeyState::KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouseX, &mouseY);

	mouseX /= SCREEN_SIZE;
	mouseY /= SCREEN_SIZE;
	mouseZ = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouseButtons[i] == KeyState::KEY_IDLE)
				mouseButtons[i] = KeyState::KEY_DOWN;
			else
				mouseButtons[i] = KeyState::KEY_REPEAT;
		}
		else
		{
			if(mouseButtons[i] == KeyState::KEY_REPEAT || mouseButtons[i] == KeyState::KEY_DOWN)
				mouseButtons[i] = KeyState::KEY_UP;
			else
				mouseButtons[i] = KeyState::KEY_IDLE;
		}
	}

	mouseXMotion = mouseYMotion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouseZ = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouseX = e.motion.x / SCREEN_SIZE;
			mouseY = e.motion.y / SCREEN_SIZE;

			mouseXMotion = e.motion.xrel / SCREEN_SIZE;
			mouseYMotion = e.motion.yrel / SCREEN_SIZE;
			break;

			// File drop mesh loading
			case (SDL_DROPFILE):
			{
				GameObject* dropedGO = app->sceneIntro->CreateGameObject("GameObject", app->sceneIntro->root);
				dropedGO->AddComponent(CompType::MESH);
				dropedGO->SetMeshInfo(app->loader->LoadMesh(e.drop.file));
				SDL_free(e.drop.file);
				TTLOG("Droped model loaded succesfully");
				break;
			}

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					app->renderer3D->OnResize(e.window.data1, e.window.data2);
			}

			
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KeyState::KEY_UP)
		return UpdateStatus::UPDATE_STOP;

	return UpdateStatus::UPDATE_CONTINUE;
}
// Called before quitting
bool ModuleInput::CleanUp()
{
	TTLOG("+++++ Quitting Module Input +++++");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}