#ifndef __APPLICATION_H__
#define __APPLICATION_H__


#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include <vector>
#include <list>
#include "p2List.h" // TODO: Change to STL



class ModuleWindow;
class ModuleInput;
class ModuleSceneIntro;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;

class Application
{
public:

	// ----- Modules -----
	
	ModuleWindow* window;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	// -------------------

	// Modules vector
	p2List<Module*> modulesList; // TODO: Change to STL

private:

	// ----- Variables used for FPS control -----

	Timer	msTimer;
	float	dt;
	// ------------------------------------------

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
};

#endif // !__APPLICATION_H__