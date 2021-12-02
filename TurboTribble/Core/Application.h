#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <vector>

#include "Timer.h"
#include "PerfTimer.h"



// ----- Forward declarations -----

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModuleViewportFrameBuffer;
class ModuleImport;
class ModuleFileSystem;
class ModuleTextures;
// --------------------------------

class Application
{
public:

	// ----- Modules -----

	ModuleWindow* window { nullptr };
	ModuleInput* input { nullptr };
	ModuleScene* scene { nullptr };
	ModuleRenderer3D* renderer3D { nullptr };
	ModuleCamera3D* camera { nullptr };
	ModuleEditor* editor { nullptr };
	ModuleViewportFrameBuffer* viewportBuffer { nullptr };
	ModuleImport* import { nullptr };
	ModuleFileSystem* fileSystem { nullptr };
	ModuleTextures* textures { nullptr };
	// -------------------

public:

	// Application Constructor
	Application();
	// Application Destructor
	~Application();

	// Initialization of the Application, this calls all Modules' Init() and Start()
	bool Init();
	// Call PreUpdate(), Update() and PostUpdate() of all Modules
	UpdateStatus Update();
	// Call CleanUp() of all Modules
	bool CleanUp();

	// Call OnGui() of all Modules and Draws Config Diagrams
	void OnGui();

private: 

	// Add a module to the list
	void AddModule(Module* mod);

	// FPS Control on Frame Start
	void PrepareUpdate();
	// FPS Control on Frame End
	void FinishUpdate();

	// Load config
	void LoadEngineConfig();
	// Save config
	void SaveEngineConfig();

	// GUI FPS Diagram
	void DrawFPSDiagram();
	// GUI Hardware
	void DrawHardwareConsole();

public:

	// ----- Engine configuration -----
	
	bool closeEngine;
	bool vsync;
	// --------------------------------
	

	// ----- FPS Count -----
	float fps;

private:

	// ----- FPS variables -----
	
	float dt;
	int cap;
	PerfTimer ptimer;
	Timer msTimer;
	Timer frameTime;
	uint64 frameCount = 0;
	Timer startupTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	// -------------------------
	

	// ----- FPS & MS Grapic -----
	
	std::vector<float> fpsLog;
	std::vector<float> msLog;
	// ---------------------------

private:

	// ----- Modules vector -----
	
	std::vector<Module*> modules;
	// --------------------------

};

extern Application* app;


#endif // !__APPLICATION_H__