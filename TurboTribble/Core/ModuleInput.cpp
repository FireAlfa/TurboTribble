#include "ModuleInput.h"

#include "Application.h"
#include "ModuleImport.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "GameObject.h"
#include "ComponentMaterial.h"

#include "Globals.h"

#include "ImGui/imgui_impl_sdl.h"

#define MAX_KEYS 300



ModuleInput::ModuleInput(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	keyboard = new KeyState[MAX_KEYS];
	memset(keyboard, (int)KeyState::KEY_IDLE, sizeof(KeyState) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	filePath = nullptr;
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	TTLOG("+++++ Loading Input Module +++++\n");
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
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch(event.type)
		{
			case SDL_MOUSEWHEEL:
				mouseZ = event.wheel.y;
			break;

			case SDL_MOUSEMOTION:
				mouseX = event.motion.x / SCREEN_SIZE;
				mouseY = event.motion.y / SCREEN_SIZE;

				mouseXMotion = event.motion.xrel / SCREEN_SIZE;
				mouseYMotion = event.motion.yrel / SCREEN_SIZE;
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
					app->renderer3D->OnResize(event.window.data1, event.window.data2);

				if (event.window.event == SDL_WINDOWEVENT_CLOSE)   // exit game
					app->closeEngine = true;

				break;
			}
			break;

			case SDL_DROPFILE:
			{
				filePath = event.drop.file;
				std::string fileName(filePath);
				if (fileName.substr(fileName.find_last_of(".")) == ".fbx" || fileName.substr(fileName.find_last_of(".")) == ".FBX" || fileName.substr(fileName.find_last_of(".")) == ".OBJ" || fileName.substr(fileName.find_last_of(".")) == ".obj")
				{
					TTLOG("~~~ Path of file dropped will be %s ~~~\n", filePath);
					app->import->LoadGeometry(filePath);
				}
				else if (fileName.substr(fileName.find_last_of(".")) == ".jpg" || fileName.substr(fileName.find_last_of(".")) == ".png" || fileName.substr(fileName.find_last_of(".")) == ".PNG" || fileName.substr(fileName.find_last_of(".")) == ".JPG")
				{
					TTLOG("~~~ Path of file dropped will be %s ~~~\n", filePath);
					std::string realFileName = fileName.substr(fileName.find_last_of("\\") + 1); 					
					if (app->textures->Find(realFileName))
					{
						TextureObject texture = app->textures->Get(realFileName);
						if (app->editor->gameobjectSelected)
						{
							if (ComponentMaterial* material = app->editor->gameobjectSelected->GetComponent<ComponentMaterial>())
							{
								material->SetTexture(texture);
							}
						}
					}
					else
					{
						TextureObject texture = app->textures->Load(realFileName);
						if (app->editor->gameobjectSelected)
						{
							if (ComponentMaterial* material = app->editor->gameobjectSelected->GetComponent<ComponentMaterial>())
							{
								material->SetTexture(texture);
							}
						}
					}
				}
			};
			SDL_free(&filePath);
			break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KeyState::KEY_UP)
		return UpdateStatus::UPDATE_STOP;

	return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	TTLOG(" +++++ Quitting Input Module +++++\n");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::OnGui()
{	
	if (ImGui::CollapsingHeader("Input"))
	{
		ImGui::Text("Mouse Position");

		ImGui::Text("X: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouseX);
		ImGui::SameLine();
		ImGui::Text("Y: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%i", mouseY);
	}
}