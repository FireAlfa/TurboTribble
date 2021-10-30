#include "ModuleWindow.h"
#include "Application.h"



// Constructor
ModuleWindow::ModuleWindow(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	window = NULL;
	screenSurface = NULL;
}
// Destructor
ModuleWindow::~ModuleWindow()
{
}


// Initialize the window, called before Render is available
bool ModuleWindow::Init()
{
	TTLOG("+++++ Init Module Window +++++");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		TTLOG("##### SDL_VIDEO could not initialize! SDL_Error: %s #####\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);

		if(window == NULL)
		{
			TTLOG("##### Window could not be created! SDL_Error: %s #####\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}
// Called before quitting
bool ModuleWindow::CleanUp()
{
	TTLOG("+++++ Quitting Module Window and all SDL systems +++++");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();

	return true;
}


// Changes the title of the window
void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}


int ModuleWindow::GetRefreshRate() {
	SDL_DisplayMode Mode;
	int DisplayIndex = SDL_GetWindowDisplayIndex(window);
	// If we can't find the refresh rate, we'll return this:
	int DefaultRefreshRate = 60;
	if (SDL_GetDesktopDisplayMode(DisplayIndex, &Mode) != 0)
	{
		return DefaultRefreshRate;
	}
	if (Mode.refresh_rate == 0)
	{
		return DefaultRefreshRate;
	}
	return Mode.refresh_rate;
}