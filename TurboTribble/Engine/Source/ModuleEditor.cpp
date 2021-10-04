#include "ModuleEditor.h"
#include "Application.h"

#include "ModuleWindow.h"

#include "Log.h"



ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}
ModuleEditor::~ModuleEditor()
{

}


bool ModuleEditor::Start()
{
	LOG("### Loading Editor ###");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->window->glContext);
	ImGui_ImplOpenGL2_Init();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Flags
	bool showDemo = false;
	bool showConsole = false;
	bool showConfig = false;
	bool showAbout = false;

	return ret;
}

update_status ModuleEditor::Update(float dt)
{
	// ImGui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Demo Window
	if(showDemo)ImGui::ShowDemoWindow();

	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC")) return UPDATE_STOP;
			ImGui::EndMenu();
		}
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

	if (showConsole)
	{
		ImGui::Begin("Console");
		ImGui::Text("This is the console");
		ImGui::End();
	}

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


	if (showAbout)
	{

		ImGui::Begin("About Turbo Tribble Engine");
		ImGui::Text("Version 0.0 - WIP");
		ImGui::Text("By Oscar Canales & Carles Garriga. Students of CITM");
		ImGui::End();
	}

	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(App->window->window);



	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{

	return true;
}