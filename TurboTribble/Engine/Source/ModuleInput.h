#ifndef __MODULE_INPUT_H__
#define __MODULE_INPUT_H__

#include "Module.h"

#define MAX_MOUSE_BUTTONS 5
#define MAX_KEYS 300



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
	bool Init();
	// Called every loop, before the logic and render are applied
	UpdateStatus PreUpdate(float dt);
	// Called before quitting
	bool CleanUp();


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

	KeyState* keyboard;
	KeyState mouseButtons[MAX_MOUSE_BUTTONS];
	int mouseX;
	int mouseY;
	int mouseZ;
	int mouseXMotion;
	int mouseYMotion;
	//int mouse_z_motion;
};

#endif // !__MODULE_INPUT_H__