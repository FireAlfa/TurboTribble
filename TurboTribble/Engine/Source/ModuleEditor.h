#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

class ModuleEditor : public Module
{
public:

	// Constructor
	ModuleEditor(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleEditor();

	// Load the Editor and create ImGui Context
	bool Start();
	// Main Update for ImGui windows
	UpdateStatus Update(float dt);
	// Render ImGui
	UpdateStatus PostUpdate(float dt);
	// CleanUp Editor
	bool CleanUp();

private:

	// ----- Flags to show specific windows -----

	bool showDemo;
	bool showConsole;
	bool showConfig;
	bool showAbout;
	bool winActive;
	bool fullscreen;
	bool resizable;
	bool borderless;
	bool fullDesktop;
	bool fileSysActive;
	bool inputActive;
	bool hardwareActive;
	// ------------------------------------------

	// Configuration variables
	char *appName = "";
	char *orgName = "";
	
};
#endif // !__MODULE_EDITOR_H__