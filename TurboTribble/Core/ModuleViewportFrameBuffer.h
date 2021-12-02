#ifndef __MODULE_VIEWPORT_FRAME_BUFFER_H__
#define __MODULE_VIEWPORT_FRAME_BUFFER_H__

#include "Module.h"

#include "Globals.h"

#include <string>



class ModuleViewportFrameBuffer : public Module
{
public:

	// Constructor
	ModuleViewportFrameBuffer(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleViewportFrameBuffer();

	// Init module
	bool Init() override;
	// Initalize the Viewport Frame Buffer
	bool Start() override;
	// Clears buffers
	UpdateStatus PreUpdate(float dt) override;
	// Binds buffers
	UpdateStatus PostUpdate(float dt) override;
	// Called before quitting
	bool CleanUp();

public:

	// ----- Viewport Fram Buffer Variables -----
	
	uint frameBuffer = 0;
	uint renderBufferoutput = 0;
	uint texture = 0;
	bool showViewportWindow = true;
	// ------------------------------------------

};

#endif // !__MODULE_VIEWPORT_FRAME_BUFFER_H__