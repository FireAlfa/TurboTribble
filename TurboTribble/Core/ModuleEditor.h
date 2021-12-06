#ifndef __MODULE_EDITOR_H__
#define __MODULE_EDITOR_H__

#include "Module.h"

#include "Globals.h"

#include "ImGui/imgui.h"
#include "SDL/include/SDL_version.h"
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


	void DrawGrid();

private:

	// Draw MenuBar
	void MenuBar();
	void PremadeObjectsMenu();

	// ----- Background Grid -----
	
	void CreateGridBuffer();
	// ---------------------------


	// ----- Draw Windows -----

	void CheckKeyboardInputs();
	void UpdateWindowStatus();

	void AboutWindow();
	void ConfigurationWindow();
	void TexturesWindow();
	void ConsoleWindow();
	void InspectorWindow();
	void InspectorGameObject();
	void HierarchyWindow();
	void GameWindow();
	void SceneWindow();
	void ResourcesWindow();
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
	bool showConfigWindow;
	bool showInspectorWindow;
	bool showHierarchyWindow;
	bool showSceneWindow;
	bool showGameWindow;
	bool showTexturesWindow;
	bool showConsoleWindow;
	bool showResourcesWindow;
	// ---------------------------------


	// ----- Style flags -----
	
	bool darkStyle;
	bool classicStyle;
	bool lightStyle;
	bool customStyle;
	// -----------------------


	// ----- About variables -----

	SDL_version SDLCompiledVersion;
	SDL_version SDLLinkedVersion;
	const char* imGuiVersion;
	int openGLMajorVersion = 0;
	int openGLMinorVersion = 0;
	SDL_version assimpVersion;
	// ---------------------------

	// Text
	ImGuiTextBuffer consoleText;
	ImVec4 currentColor;

	// Scene
	ImGuiWindowFlags sceneWindow = 0;
	ImVec2 sceneLastViewportSize;
	ImVec2 gameLastViewportSize;

};

#endif // !__MODULE_EDITOR_H__