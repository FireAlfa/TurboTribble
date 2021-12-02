#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include "Globals.h"

#include "ImGui/imgui.h"
#include <string>



class GameObject;

class ModuleEditor : public Module
{
private:

	struct Grid
	{
		uint VAO;
		uint length;

		~Grid();
	};

	Grid grid;

public:

	// Constructor
	ModuleEditor(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleEditor();

	// Initialize the Editor
	bool Init() override;
	// Load the Editor and create ImGui Context
	bool Start() override;
	// Start ImGui Frame
	UpdateStatus PreUpdate(float dt) override;
	// Updates each window
	UpdateStatus Update(float dt) override;
	// Render ImGui
	UpdateStatus PostUpdate(float dt) override;
	// Called before quitting
	bool CleanUp();

	// Console Text Pushback
	void UpdateText(const char* consoleText);

private:

	// Draw MenuBar
	void MenuBar();


	// ----- Background Grid -----
	
	void CreateGridBuffer();
	void DrawGrid();
	// ---------------------------


	// ----- Draw Windows -----

	void UpdateWindowStatus();
	void AboutWindow();	// Can be done better
	void InspectorGameObject();
	// ------------------------


	// ----- Docking Helper functions -----
	
	bool DockingRootItem(char* id, ImGuiWindowFlags winFlags);
	void BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size = { .0f, .0f });
	// ------------------------------------

public:

	// Current selected GameObject
	GameObject* gameobjectSelected;

private:

	// ----- Window status control -----
	
	bool showDemoWindow;
	bool showAnotherWindow;
	bool showAboutWindow;
	bool showConfWindow;
	bool showInspectorWindow;
	bool showHierarchyWindow;
	bool showSceneWindow;
	bool showGameWindow;
	bool showTextures;
	bool showConsoleWindow;
	// ---------------------------------


	// Text
	ImGuiTextBuffer consoleText;
	ImVec4 currentColor;

	// Scene
	ImGuiWindowFlags sceneWindow = 0;
	ImVec2 lastViewportSize;

};

#endif // !__MODULE_EDITOR_H__