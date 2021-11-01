#ifndef __APPLICATION_H__
#define __APPLICATION_H__


#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include <vector>
#include <list>



class ModuleWindow;
class ModuleInput;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;
class ModuleMeshLoader;

class Application
{
public:

	// ----- Modules -----
	
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* sceneIntro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleMeshLoader* loader;
	// -------------------

	// Modules vector
	std::vector<Module*> modulesList;

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


private:

	// Called each loop before calling the Modules' Update() methods
	void PrepareUpdate();
	// Called each loop after calling the Modules' Update() methods
	void FinishUpdate();


	// Adds a Module to the Module vector
	void AddModule(Module* mod);


private:

	// ----- Variables used for FPS control -----

	Timer	msTimer;
	float	dt;
	// ------------------------------------------

public:

	// ----- Variables used for FPS control -----

	int maxFPS = MAX_FPS;
	// ------------------------------------------
};

#endif // !__APPLICATION_H__