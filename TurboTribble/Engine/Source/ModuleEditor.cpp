#include "ModuleEditor.h"
#include "Application.h"

#include <vector>
#include <gl/GL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
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

	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->renderer3D->glContext);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	return ret;
}
// Main Update for ImGui windows
UpdateStatus ModuleEditor::Update(float dt)
{
	// ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Demo Window
	if (showDemo)ImGui::ShowDemoWindow(&showDemo);

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
			if (ImGui::MenuItem("Gui Demo", 0, showDemo)) { showDemo = !showDemo; }
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
		ImGui::Separator();
		ImGui::Text("3rd Party Libraries used:");
		ImGui::BulletText("SLD 2.0.0");
		ImGui::BulletText("Glew 2.0.0");
		ImGui::BulletText("ImGui 1.85");
		ImGui::BulletText("MathGeoLib 1.5");
		ImGui::BulletText("OpenGL 3.1");
		ImGui::BulletText("Assimp 5.0.1 ");
		ImGui::Separator();
		ImGui::Text("License:");
		ImGui::Text("MIT License");
		ImGui::TextWrapped(
			"Copyright(c) 2021 Oscar Canales and Carles Garriga"
			"Permission is hereby granted, free of charge, to any person obtaining a copy"
			"of this softwareand associated documentation files(the Software), to deal"
			"in the Software without restriction, including without limitation the rights"
			"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell"
			"copies of the Software, and to permit persons to whom the Software is"
			"furnished to do so, subject to the following conditions :"
			
			"The above copyright noticeand this permission notice shall be included in all"
			"copies or substantial portions of the Software."

			"THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE"
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE"
			"SOFTWARE.");

		ImGui::End();
	}


	return UpdateStatus::UPDATE_CONTINUE;
}

// Render ImGui
UpdateStatus ModuleEditor::PostUpdate(float dt)
{
	// ImGui Rendering
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


	return UpdateStatus::UPDATE_CONTINUE;
}

// CleanUp Editor
bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}