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
#include "ModuleInput.h"



// Constructor
ModuleEditor::ModuleEditor(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	// Flags reset
	showDemo = false;
	showConsole = false;
	showConfig = false;
	showAbout = false;
	winActive = true;
	fullscreen = false;
	resizable = false;
	borderless = false;
	fullDesktop = false;
	fileSysActive = true;
	inputActive = true;
	hardwareActive = true;

	appName = "Turbo Tribble";
	orgName = "CITM UPC";
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
			if (ImGui::InputText("App name", appName, 20)) { SDL_SetWindowTitle(app->window->window, appName); }

			ImGui::InputText("Organization", orgName, 20);
			
			// TODO FPS stuff
				//ImGui::PlotHistogram for the fps graphs
		}
		if (ImGui::CollapsingHeader("Window"))
		{
			// Window active checkbox
			if (ImGui::Checkbox("Active", &winActive));
			
			// Brightness
			float brightnessVal = SDL_GetWindowBrightness(app->window->window);
			if (ImGui::SliderFloat("Brightness", &brightnessVal, 0, 1)) { SDL_SetWindowBrightness(app->window->window, brightnessVal); }

			// Width & Height
			int width, height;
			SDL_GetWindowSize(app->window->window, &width, &height);
			if (ImGui::SliderInt("Width", &width, 640, 1920)) { SDL_SetWindowSize(app->window->window, width, height); }
			if (ImGui::SliderInt("Height", &height, 480, 1080)) { SDL_SetWindowSize(app->window->window, width, height); }

			// TODO Refresh rate well
			ImGui::Text("Refresh rate:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "60");


			// Window Checkboxes
			if (ImGui::Checkbox("Fullscreen", &fullscreen)) { SDL_SetWindowFullscreen(app->window->window, fullscreen); }
			ImGui::SameLine();
			if (ImGui::Checkbox("Resizable", &resizable)) {/* Do resizable stuff here*/}

			if (ImGui::Checkbox("Borderless", &borderless)) { SDL_SetWindowBordered(app->window->window, (SDL_bool)borderless); }
			ImGui::SameLine();
			if (ImGui::Checkbox("Full Desktop", &fullDesktop)) { SDL_SetWindowFullscreen(app->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP); }
				
		}
		if (ImGui::CollapsingHeader("File System"))
		{
			if (ImGui::Checkbox("Active", &fileSysActive));

			ImGui::Text("Base Path:");
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", SDL_GetBasePath());
			ImGui::Text("Read Path:");
			ImGui::TextColored(ImVec4(1, 1, 0, 1), ".");
			ImGui::Text("Write Path:");
			ImGui::TextColored(ImVec4(1, 1, 0, 1), ".");
			
		}
		if (ImGui::CollapsingHeader("Input"))
		{
			if (ImGui::Checkbox("Active", &inputActive));

			ImGui::Text("Mouse Position:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d, %d", app->input->GetMouseX(), app->input->GetMouseY());
			ImGui::Text("Mouse Motion:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d, %d", app->input->GetMouseXMotion(), app->input->GetMouseYMotion());
			ImGui::Text("Mouse Wheel:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", app->input->GetMouseZ());

			//TODO Input LOG List

		}
		if (ImGui::CollapsingHeader("Hardware"))
		{
			if (ImGui::Checkbox("Active:", &hardwareActive));

			SDL_version versionSDL;
			SDL_GetVersion(&versionSDL);
			ImGui::Text("SDL Version:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d, %d, %d", versionSDL.major, versionSDL.minor, versionSDL.patch);
			ImGui::Separator();
			ImGui::Text("CPU:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %dKb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());;
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%.1fGb", (float)SDL_GetSystemRAM());
			ImGui::Text("Caps:");

			if (SDL_HasRDTSC() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "RDTSC,"); }
			if (SDL_HasMMX()   == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "MMX,");   }
			if (SDL_HasSSE()   == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE,");   }
			if (SDL_HasSSE2()  == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE2,");  }
			if (SDL_HasSSE3()  == SDL_bool::SDL_TRUE) { ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE3,");					 }
			if (SDL_HasSSE41() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE41,"); }
			if (SDL_HasSSE42() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE42,"); }
			if (SDL_HasAVX()   == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX,");   }
			if (SDL_HasAVX2()  == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX2,");  }

			ImGui::Separator();
			ImGui::Text("GPU:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", glGetString(GL_VENDOR));
			ImGui::Text("Brand:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", glGetString(GL_RENDERER));

			// TODO VRAM numbers
			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d Mb");
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d Mb");
			ImGui::Text("VRAM Avaliable:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d Mb");
			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d Mb");
			
		}
		ImGui::End();
	}

	// GUI About Window
	if (showAbout)
	{

		ImGui::Begin("About Turbo Tribble Engine");
		ImGui::Text("Turbo Tribble is a C++ Game Engine developed as a class project");
		ImGui::Text("Version 0.0 - WIP");
		ImGui::Text("By Oscar Canales & Carles Garriga. Students of CITM");
		ImGui::Text("3rd Party Libraries used:");
		ImGui::Text("- SLD 2.0.0");
		ImGui::Text("- Glew 2.0.0");
		ImGui::Text("- ImGui 1.85");
		ImGui::Text("- MathGeoLib 1.5");
		ImGui::Text("- OpenGL 2.1");
		ImGui::Text("- Assimp 5.0.1 ");
		ImGui::Text("License:");
		ImGui::Text("MIT License ");
		ImGui::Text("Copyright(c) 2021 Òscar Canales and Carles Garriga");
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