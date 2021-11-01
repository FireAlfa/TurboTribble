#include "ModuleEditor.h"
#include "Application.h"

#include "glew.h"
#include "imgui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl3.h"
#include "SDL/include/SDL_opengl.h"
#include "Assimp/include/version.h"
#include <gl/GL.h>

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "ModuleSceneIntro.h"



// Constructor
ModuleEditor::ModuleEditor(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	// Flags reset
	showDemo = false;
	showConsole = false;
	showConfig = false;
	showAbout = false;
	showHierarchy = false;
	showInspector = false;
	winActive = true;
	fullscreen = false;
	resizable = true;
	borderless = false;
	fullDesktop = false;
	fileSysActive = true;
	inputActive = true;
	hardwareActive = true;

	// ----------------

	depthTest = true;
	cullFace = true;
	lighting = true;
	colorMaterial = true;
	texture2D = true;

	//appName = "TurboTribble";
	//orgName = "CITM-UPC";
}
// Destructor
ModuleEditor::~ModuleEditor()
{

}


// Load the Editor and create ImGui Context
bool ModuleEditor::Start()
{
	TTLOG("+++++ Loading Editor +++++");
	bool ret = true;

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

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

	// Get SDL version
	SDL_VERSION(&SDLCompiledVersion);
	SDL_GetVersion(&SDLLinkedVersion);
	// Get ImGui version
	imGuiVersion = ImGui::GetVersion();
	// Get OpenGL version
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &openGLMajorVersion);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &openGLMinorVersion);
	// Get Assimp version
	assimpVersion.major = aiGetVersionMajor();
	assimpVersion.minor = aiGetVersionMinor();
	assimpVersion.patch = aiGetVersionRevision();

	return ret;
}
// Main Update for ImGui windows
UpdateStatus ModuleEditor::Update(float dt)
{
	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;

	// ImGui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Check for shortcuts that open certain windows
	CheckKeyboardInputs();

	// Demo Window
	if (showDemo)ImGui::ShowDemoWindow(&showDemo);

	// Menu Bar
	ret = MenuBar();
	

	// GUI Hierarchy Window
	if (showHierarchy)
	{
		ImGui::Begin("Hierarchy", &showHierarchy);
		HierarchyWindow(app->sceneIntro->root);
		ImGui::End();
	}

	// GUI Inspector Window
	if (showInspector) { InspectorWindow(); };

	// GUI Console Window
	if (showConsole) { ConsoleWindow(); };

	// GUI Config Window
	if (showConfig) { ConfigurationWindow(); };

	// GUI About Window
	if (showAbout) { AboutWindow(); };


	return ret;
}

// Render ImGui
UpdateStatus ModuleEditor::PostUpdate(float dt)
{
	// Nothing to do here yet

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


// Draw everything on screen, called by ModuleRenderer
void ModuleEditor::Draw()
{
	// ImGui Rendering
	ImGui::Render();
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

// Change flags to show specific windows via keyboard
void ModuleEditor::CheckKeyboardInputs()
{
	if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN))
	{
		showHierarchy = !showHierarchy;
	}

	if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN))
	{
		showInspector = !showInspector;
	}

	if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN))
	{
		showConsole = !showConsole;
	}

	if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
		app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_DOWN ||
			app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) &&
		(app->input->GetKey(SDL_SCANCODE_O) == KeyState::KEY_DOWN))
	{
		showConfig = !showConfig;
	}
}

// Display the Menu Bar
UpdateStatus ModuleEditor::MenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		// Menu Bar File Tab
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit", "ESC")) return UpdateStatus::UPDATE_STOP;
			ImGui::EndMenu();
		}
		// Menu Bar Edit Tab
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Configuration", "Ctrl+Shift+O", showConfig))
			{
				showConfig = !showConfig;
			}
			ImGui::EndMenu();
		}
		// Menu Bar GameObject Tab
		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::BeginMenu("3D Objects"))
			{
				// Primitives menu
				if (ImGui::MenuItem("Cube"))
				{
					// TODO Create Cube
					GameObject* cube = app->sceneIntro->CreateGameObject("Cube", app->sceneIntro->root);
					Component* mesh = cube->AddComponent(CompType::MESH);
					//mesh->GetMeshInfo() = app->loader->LoadMesh("");
				}
				if (ImGui::MenuItem("Sphere"))
				{
					// TODO Create Sphere
				}
				if (ImGui::MenuItem("Cylinder"))
				{
					// TODO Create Cylinder
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		// Menu Bar Window Tab
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::BeginMenu("General"))
			{
				if (ImGui::MenuItem("Hierarchy", "Ctrl+1", showHierarchy))
				{
					showHierarchy = !showHierarchy;
				}
				if (ImGui::MenuItem("Inspector", "Ctrl+2", showInspector))
				{
					showInspector = !showInspector;
				}
				if (ImGui::MenuItem("Console", "Ctrl+Shift+C", showConsole))
				{
					showConsole = !showConsole;
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		// Menu Bar Help Tab
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About TurboTribble")) { showAbout = !showAbout, SW_SHOW; }
			ImGui::Separator();
			if (ImGui::MenuItem("Documentation")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/wiki", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Download Latest")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/releases", 0, 0, SW_SHOW); }
			if (ImGui::MenuItem("Report a Bug")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/issues", 0, 0, SW_SHOW); }
			ImGui::Separator();
			if (ImGui::MenuItem("Gui Demo", 0, showDemo)) { showDemo = !showDemo; }

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	return UpdateStatus::UPDATE_CONTINUE;
}

// Display the Console
void ModuleEditor::ConsoleWindow()
{
	ImGui::Begin("Console", &showConsole);
	for (int i = 0; i < consoleLogs.size(); i++)
	{
		ImGui::Text("%s", consoleLogs.at(i).c_str());
	}
	ImGui::End();
}

// Display the Configuration
void ModuleEditor::ConfigurationWindow()
{
	ImGui::Begin("Configuration", &showConfig);

	if (ImGui::BeginMenu("Options"))
	{

		ImGui::MenuItem("Set Defaults");
		ImGui::MenuItem("Load");
		ImGui::MenuItem("Save");

		ImGui::EndMenu();

	}

	
	if (ImGui::CollapsingHeader("Application"))
	{
		// Select all text when you enter the box and only modify it when Enter is pressed
		ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
		char text[32];
		// App name box
		strcpy_s(text, appName);
		if (ImGui::InputText("App Name", text, IM_ARRAYSIZE(text), textFlags))
		{
			strcpy_s(appName, text);
			SDL_SetWindowTitle(app->window->window, appName);
		}
		// Organization name box
		strcpy_s(text, orgName);
		if (ImGui::InputText("Organization", text, IM_ARRAYSIZE(text), textFlags))
		{
			strcpy_s(orgName, text);
		}

		fpsLog.push_back(ImGui::GetIO().Framerate);
		msLog.push_back(1000.0f / ImGui::GetIO().Framerate);
		//mem_log.push_back(ImGui::GetIO().);

		// FPS Slider
		ImGui::SliderInt("Max FPS", &app->maxFPS, 1, MAX_FPS);
		ImGui::Text("Limit Framerate");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", app->maxFPS);

		// FPS Histogram
		sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
		ImGui::PlotHistogram("##framerate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 170.0f, ImVec2(310, 100));
		if (fpsLog.size() > 70.0f)
		{
			fpsLog.erase(fpsLog.begin());
		}

		// MS Histogram
		sprintf_s(title, 25, "Milliseconds %0.1f", msLog[msLog.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &msLog[0], msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
		if (msLog.size() > 70.0f)
		{
			msLog.erase(msLog.begin());
		}

		// TODO Memory Consumption Histogram
		//sprintf_s(title, 25, "Memory Usage %.1f", mem_log[mem_log.size() - 1]);
		//ImGui::PlotHistogram("##memory", &mem_log[0], mem_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		ImGui::Text("Total Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Total Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Peak Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Peak Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Accumulated Reported Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Accumulated Actual Mem: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Accumulated Alloc Unit Count: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Total Alloc Unit Count: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);
		ImGui::Text("Peak Alloc Unit Count: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(255, 255, 0, 255), "%d", 1);

	}
	if (ImGui::CollapsingHeader("Window"))
	{
		// Brightness
		float brightnessVal = SDL_GetWindowBrightness(app->window->window);
		if (ImGui::SliderFloat("Brightness", &brightnessVal, 0, 1)) { SDL_SetWindowBrightness(app->window->window, brightnessVal); }

		// Width & Height
		int width, height;
		SDL_GetWindowSize(app->window->window, &width, &height);
		if (ImGui::SliderInt("Width", &width, 640, 1920)) { SDL_SetWindowSize(app->window->window, width, height); }
		if (ImGui::SliderInt("Height", &height, 480, 1080)) { SDL_SetWindowSize(app->window->window, width, height); }

		// Refresh rate
		ImGui::Text("Refresh rate:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d", app->window->GetRefreshRate());


		// Window Checkboxes
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) { SDL_SetWindowFullscreen(app->window->window, fullscreen); }
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable)) {/* TODO change whether window is resizable or not */ }

		if (ImGui::Checkbox("Borderless", &borderless)) { SDL_SetWindowBordered(app->window->window, (SDL_bool)!borderless); }
		ImGui::SameLine();
		if (ImGui::Checkbox("Full Desktop", &fullDesktop)) { SDL_SetWindowFullscreen(app->window->window, fullDesktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : false); }

	}
	if (ImGui::CollapsingHeader("File System"))
	{
		if (ImGui::Checkbox("Active", &fileSysActive)) {};

		ImGui::Text("Base Path:");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", SDL_GetBasePath());
		ImGui::Text("Read Path:");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), ".");
		ImGui::Text("Write Path:");
		ImGui::TextColored(ImVec4(1, 1, 0, 1), ".");

	}
	if (ImGui::CollapsingHeader("Input"))
	{
		if (ImGui::Checkbox("Active", &inputActive)) {};

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
		if (ImGui::Checkbox("Active:", &hardwareActive)) {};

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
		if (SDL_HasMMX() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "MMX,"); }
		if (SDL_HasSSE() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE,"); }
		if (SDL_HasSSE2() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE2,"); }
		if (SDL_HasSSE3() == SDL_bool::SDL_TRUE) { ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE3,"); }
		if (SDL_HasSSE41() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE41,"); }
		if (SDL_HasSSE42() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE42,"); }
		if (SDL_HasAVX() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX,"); }
		if (SDL_HasAVX2() == SDL_bool::SDL_TRUE) { ImGui::SameLine(); ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX2,"); }

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
	if (ImGui::CollapsingHeader("Renderer"))
	{
		if (ImGui::Checkbox("Depth Test", &depthTest))
		{
			if (depthTest == true) glEnable(GL_DEPTH_TEST);
			else glDisable(GL_DEPTH_TEST);
		}
		if (ImGui::Checkbox("Cull Face", &cullFace))
		{
			if (cullFace == true) glEnable(GL_CULL_FACE);
			else glDisable(GL_CULL_FACE);
		}
		if (ImGui::Checkbox("Lighting", &lighting))
		{
			if (lighting == true) glEnable(GL_LIGHTING);
			else glDisable(GL_LIGHTING);
		}
		if (ImGui::Checkbox("Color Material", &colorMaterial))
		{
			if (colorMaterial == true) glEnable(GL_COLOR_MATERIAL);
			else glDisable(GL_COLOR_MATERIAL);
		}
		if (ImGui::Checkbox("Texture 2D", &texture2D))
		{
			if (texture2D == true) glEnable(GL_TEXTURE_2D);
			else glDisable(GL_TEXTURE_2D);
		}
	}
	ImGui::End();
}

// Display the About Window
void ModuleEditor::AboutWindow()
{
	ImGui::Begin("About TurboTribble Engine", &showAbout);
	ImGui::Text("TurboTribble is a C++ Game Engine developed as a class project.");
	ImGui::Text("Version 0.1 - WIP");
	ImGui::Text("By Oscar Canales & Carles Garriga. Students of CITM-UPC.");
	ImGui::Separator();
	ImGui::Text("3rd Party Libraries used:");
	ImGui::BulletText("Compiled SDL %d.%d.%d", SDLCompiledVersion.major, SDLCompiledVersion.minor, SDLCompiledVersion.patch);
	ImGui::BulletText("Linked SDL %d.%d.%d", SDLLinkedVersion.major, SDLLinkedVersion.minor, SDLLinkedVersion.patch);
	ImGui::BulletText("Glew %s", glewGetString(GLEW_VERSION));
	ImGui::BulletText("ImGui %s", imGuiVersion);
	ImGui::BulletText("MathGeoLib 1.5");
	ImGui::BulletText("OpenGL %d.%d", openGLMajorVersion, openGLMinorVersion);
	ImGui::BulletText("Assimp %d.%d.%d", assimpVersion.major, assimpVersion.minor, assimpVersion.patch);
	ImGui::Separator();
	ImGui::Text("License:");
	ImGui::Text("MIT License");
	ImGui::Text("Copyright(c) 2021 Oscar Canales and Carles Garriga");
	ImGui::TextWrapped(
		"Permission is hereby granted, free of charge, to any person obtaining a copy "
		"of this software and associated documentation files(the Software), to deal "
		"in the Software without restriction, including without limitation the rights "
		"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell "
		"copies of the Software, and to permit persons to whom the Software is "
		"furnished to do so, subject to the following conditions:");
	ImGui::TextWrapped(
		"The above copyright notice and this permission notice shall be included in all "
		"copies or substantial portions of the Software.");
	ImGui::TextWrapped(
		"THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR "
		"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "
		"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE "
		"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER "
		"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
		"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE "
		"SOFTWARE.");

	ImGui::End();
}

// Display the Tree part of the Hierarchy Window
void ModuleEditor::HierarchyWindow(GameObject* go)
{
	if (go != nullptr)
	{
		// You need to click on the arrow to open it and if the GameObject has no children, don't give the option to open it
		ImGuiTreeNodeFlags parentFlags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | (go->children.empty() ? ImGuiTreeNodeFlags_Leaf : 0);

		// Give the node the Selected flag
		if (go == selectedNode)
		{
			parentFlags |= ImGuiTreeNodeFlags_Selected;
		}

		// Create the node, returns true if it's opened
		bool open = ImGui::TreeNodeEx(go->name.c_str(), parentFlags);

		// Control what GameObject we have selected with the mouse
		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left))
		{
			selectedNode = go;
		}

		// Recursive call to this method for each children
		if (open) {
			for (size_t i = 0; i < go->children.size(); i++)
			{
				HierarchyWindow(go->children.at(i));
			};
			ImGui::TreePop();
		}
	}
}

void ModuleEditor::InspectorWindow()
{
	ImGui::Begin("Inspector", &showInspector);
	if (app->sceneIntro->root != nullptr)
	{
		for (int i = 0; i < app->sceneIntro->root->children.size(); i++)
		{
			if (app->sceneIntro->root->children.at(i) == selectedNode)
			{
				for (int j = 0; j < app->sceneIntro->root->children.at(i)->components.size(); j++)
				{
					app->sceneIntro->root->children.at(i)->components.at(j)->DrawInspector();
				}
			}
		}
	}
	ImGui::End();
}