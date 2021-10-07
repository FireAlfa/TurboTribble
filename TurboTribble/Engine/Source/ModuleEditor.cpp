#include "ModuleEditor.h"
#include "Application.h"

#include <vector>
#include <gl/GL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "SDL/include/SDL_opengl.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"



// Constructor
ModuleEditor::ModuleEditor(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	// Flags reset
	showDemo = false;
	showConsole = false;
	showConfig = false;
	showAbout = false;
}
// Destructor
ModuleEditor::~ModuleEditor()
{

}


// Load the Editor and create ImGui Context
bool ModuleEditor::Start()
{
	LOG("### Loading Editor ###");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->renderer3D->glContext);
	ImGui_ImplOpenGL2_Init();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	return ret;
}
// Main Update for ImGui windows
UpdateStatus ModuleEditor::Update(float dt)
{
	// ImGui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Demo Window
	if (showDemo)ImGui::ShowDemoWindow();

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		// Menu Bar File Tab
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC")) return UpdateStatus::UPDATE_STOP;
			ImGui::EndMenu();
		}
		// Menu Bar View Tab
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Console", "1", showConsole))
			{
				showConsole = !showConsole;
			}
			if (ImGui::MenuItem("Configuration", "4", showConfig))
			{
				showConfig = !showConfig;
			}
			ImGui::EndMenu();
		}
		// Menu Bar Help Tab
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo", 0, showDemo))
			{
				showDemo = !showDemo;
			}
			if (ImGui::MenuItem("Documentation")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/wiki", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Download Latest")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/releases", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Report a Bug")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/issues", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("About")) { showAbout = !showAbout, SW_SHOW; }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	// GUI Console Window
	if (showConsole)
	{
		ImGui::Begin("Console");
		ImGui::Text("This is the console");
		ImGui::End();
	}

	// GUI Console Window
	if (showConfig)
	{
		ImGui::Begin("Configuration");
		ImGui::Text("Options");
		if (ImGui::CollapsingHeader("Application"))
		{

		}
		if (ImGui::CollapsingHeader("Window"))
		{

		}
		if (ImGui::CollapsingHeader("File System"))
		{

		}
		if (ImGui::CollapsingHeader("Input"))
		{

		}
		if (ImGui::CollapsingHeader("Hardware"))
		{

		}
		ImGui::End();
	}

	// GUI About Window
	if (showAbout)
	{

		ImGui::Begin("About Turbo Tribble Engine");
		ImGui::Text("Version 0.0 - WIP");
		ImGui::Text("By Oscar Canales & Carles Garriga. Students of CITM");
		ImGui::End();
	}

	// ImGui Rendering
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return UpdateStatus::UPDATE_CONTINUE;
}
// CleanUp Editor
bool ModuleEditor::CleanUp()
{

	return true;
}