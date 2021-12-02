#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"

#include "SDL/include/SDL.h"



class Application;

class ModuleWindow : public Module
{
public:

	// Constructor
	ModuleWindow(Application* app, bool startEnabled = true);
	// Destructor
	virtual ~ModuleWindow();

	// Initialize the window, called before Render is available
	bool Init() override;
	// Set Window settings according to flags
	bool Start() override;
	// Called before quitting, destroys the window
	bool CleanUp() override;

	// Load Window Info
	void OnLoad(const JSONReader& reader) override;
	// Save Window Info
	void OnSave(JSONWriter& writer) const override;

	// Draw Camera Info
	void OnGui() override;
	// Change the Window's title
	void SetTitle(const char* title);

public:

	// The window we'll be rendering to
	SDL_Window* window;

	// The surface contained by the window
	SDL_Surface* screenSurface;

	SDL_GLContext context;


	// ----- ImGui control variables -----
	
	Uint32 flags;
	int width;
	int height;
	float brightness;
	bool resizable = true;
	bool fullscreen = false;
	bool borderless = true;
	bool fullDesktop = true;
	// -----------------------------------
	

	// Aspect Ratio
	float windowAspectRatio;

};

#endif // !__MODULE_WINDOW_H__