#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include "SDL/include/SDL_version.h"
#include <string>
#include <vector>



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


	// Draw everything on screen, called by ModuleRenderer
	void Draw();

private:

	void CheckKeyboardInputs();
	UpdateStatus MenuBar();
	void ConsoleWindow();
	void ConfigurationWindow();
	void AboutWindow();

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


	// ----- Configuration variables -----

	char appName[32] = "TurboTribble";
	char orgName[32] = "CITM-UPC";
	// -----------------------------------
	

	// ----- About variables -----

	SDL_version SDLCompiledVersion;
	SDL_version SDLLinkedVersion;
	const char* imGuiVersion;
	int openGLMajorVersion = 0;
	int openGLMinorVersion = 0;
	SDL_version assimpVersion;
	// ---------------------------


	// ----- FPS/MS Graph -----
	
	char title[25];
	std::vector<float> fpsLog;
	std::vector<float> msLog;
	std::vector<float> memLog;
	// ------------------------
};
#endif // !__MODULE_EDITOR_H__