#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "Globals.h"
#include "Timer.h"

#include <vector>
#include <list>


class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleEditor;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;

	std::vector<Module*> modulesList;

private:

	Timer msTimer;
	float dt;

public:

	int maxFPS;
	int screenRefresh;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetDeltaTime() const { return dt; }
	float GetFrameRate() const { return 1.0f / dt; }

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;
#endif // !__APP_H__