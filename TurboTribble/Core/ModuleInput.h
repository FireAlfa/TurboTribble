#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5



enum class KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:

	// Constructor
	ModuleInput(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleInput();

	// Initialize the input events subsystem
	bool Init() override;
	// Called every loop, before the logic and render are applied
	UpdateStatus PreUpdate(float dt) override;
	// Called before quitting
	bool CleanUp() override;

	// Draw Input info
	void OnGui();
	//void ImGuiEventInput();


	// ----- Getters -----

	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	KeyState GetMouseButton(int id) const
	{
		return mouseButtons[id];
	}

	int GetMouseX() const
	{
		return mouseX;
	}

	int GetMouseY() const
	{
		return mouseY;
	}

	int GetMouseZ() const
	{
		return mouseZ;
	}

	int GetMouseXMotion() const
	{
		return mouseXMotion;
	}

	int GetMouseYMotion() const
	{
		return mouseYMotion;
	}
	// -------------------

private:

	// ----- Input Variables -----

	KeyState* keyboard;
	KeyState mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	int mouseXMotion;
	int mouseYMotion;
	int mouseZMotion;
	// ---------------------------

public:

	const char* filePath;
};

#endif // !__MODULE_INPUT_H__