#ifndef __MODULE_WINDOW_H__
#define __MODULE_WINDOW_H__

#include "Module.h"

#include "SDL/include/SDL.h"



class ModuleWindow : public Module
{
public:

	// Constructor
	ModuleWindow(Application* app, bool startEnabled = true);
	// Destructor
	virtual ~ModuleWindow();

	// Initialize the window, called before Render is available
	bool Init();
	// Called before quitting, destroys the window
	bool CleanUp();


	// Changes the title of the window
	void SetTitle(const char* title);

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screenSurface;
};

#endif // __MODULE_WINDOW_H__