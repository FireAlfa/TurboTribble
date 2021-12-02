#include "Application.h"
#include "p2Defs.h"

#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/MathBuildConfig.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleViewportFrameBuffer.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "Globals.h"



Application::Application()
{
	PERF_START(ptimer);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	viewportBuffer = new ModuleViewportFrameBuffer(this);
	import = new ModuleImport(this);
	fileSystem = new ModuleFileSystem(this);
	textures = new ModuleTextures(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(fileSystem);
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(textures);
	AddModule(import);
	
	// Scenes
	AddModule(viewportBuffer);
	AddModule(scene);
	AddModule(editor);

	// Renderer last!
	AddModule(renderer3D);

	// Control variable to close app
	closeEngine = false;
	vsync = false;
	fps = 0.0f;
	cap = 60;

	PERF_PEEK(ptimer);
}

Application::~Application()
{

	for(uint i = modules.size(); i <= 0 ; i--)
		RELEASE(modules[i]);
	
	modules.clear();

}

bool Application::Init()
{
	PERF_START(ptimer);
	bool ret = true;

	// Call Init() in all modules
	for (size_t i = 0; i < modules.size() && ret == true; i++)
	{
		ret = modules[i]->Init();
	}

	LoadEngineConfig();

	// After all Init calls we call Start() in all modules
	TTLOG("++++++++ Application Start ++++++++\n");
	for (size_t i = 0; i < modules.size() && ret == true; i++)
	{
		ret = modules[i]->Start();
	}
	
	msTimer.Start();
	PERF_PEEK(ptimer);

	return ret;
}

void Application::LoadEngineConfig()
{
	char* buffer = nullptr;
	uint bytesFile = fileSystem->Load("engine_config.cfg", &buffer);

	if (bytesFile)
	{
		rapidjson::Document document;
		if (document.Parse<rapidjson::kParseStopWhenDoneFlag>(buffer).HasParseError())
		{
			TTLOG("######## Error loading engine config ########\n");
		}
		else
		{
			const rapidjson::Value config = document.GetObjectJSON();

			for (size_t i = 0; i < modules.size(); i++)
			{
				modules[i]->OnLoad(config);
			}

			TTLOG("++++++++ Engine config loaded ++++++++\n");
		}
	}
	RELEASE_ARRAY(buffer);
}

// ---------------------------------------------
void Application::PrepareUpdate()
{

	frameCount++;
	lastSecFrameCount++;

	dt = (float)frameTime.ReadSec();
	frameTime.Start();

}

// ---------------------------------------------
void Application::FinishUpdate()
{

	// Framerate calculations --
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		fps = static_cast<float>(lastSecFrameCount);
		lastSecFrameCount = 0;
	}

	uint32 lastFrameMS = frameTime.Read();

}

void Application::OnGui()
{
	if (ImGui::CollapsingHeader("Application"))
		DrawFPSDiagram();

	if (ImGui::CollapsingHeader("Hardware"))
		DrawHardwareConsole();

	for (Module* module : modules)
	{
		module->OnGui();
	}

}

// Call PreUpdate, Update and PostUpdate on all modules
UpdateStatus Application::Update()
{

	UpdateStatus ret = UpdateStatus::UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (size_t i = 0; i < modules.size() && ret== UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PreUpdate(dt);
	}

	for (size_t i = 0; i < modules.size() && ret == UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->Update(dt);
	}


	for (size_t i = 0; i < modules.size() && ret == UpdateStatus::UPDATE_CONTINUE; i++)
	{
		ret = modules[i]->PostUpdate(dt);

	}

	// If main menu bar exit button pressed changes closeEngine bool to true and closes app
	if (closeEngine) ret = UpdateStatus::UPDATE_STOP;

	FinishUpdate();

	return ret;
}



bool Application::CleanUp()
{
	bool ret = true;
	SaveEngineConfig();

	for (size_t i = 0; i < modules.size() && ret == true; i++)
	{
		ret = modules[i]->CleanUp();
	}

	return ret;
}

void Application::SaveEngineConfig()
{
	rapidjson::StringBuffer sb;
	JSONWriter writer(sb);

	writer.StartObject();
	for (size_t i = 0; i < modules.size(); i++)
	{
		modules[i]->OnSave(writer);
	}
	writer.EndObject();

	if (fileSystem->Save("engine_config.cfg", sb.GetString(), strlen(sb.GetString()), false))
	{
		TTLOG("++++++++ Engine configuration saved. ++++++++\n");
	}
	else
	{
		TTLOG("######## Engine configuration not saved. ########\n");
	}
}

void Application::AddModule(Module* mod)
{
	modules.push_back(mod);
}

void Application::DrawFPSDiagram() {

	ImGui::InputText("App Name", TITLE, 20);
	ImGui::InputText("Organization", ORGANITZATION, 20);
	ImGui::SliderInt("Framerate", &cap, -1, 120);

	if (fpsLog.size() != 30)
	{
		fpsLog.push_back(fps);
		msLog.push_back(dt * 1000);
	}
	else
	{
		fpsLog.erase(fpsLog.begin());
		fpsLog.push_back(fps);
		msLog.erase(msLog.begin());
		msLog.push_back(dt * 1000);
	}

	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLog.size() - 1]);
	ImGui::PlotHistogram("##framerate", &fpsLog[0], fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", msLog[msLog.size() - 1]);
	ImGui::PlotHistogram("##framerate", &msLog[0], msLog.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	if (ImGui::Checkbox("VSYNC:", &renderer3D->vsyncActive)) {

		if (renderer3D->vsyncActive)
			SDL_GL_SetSwapInterval(1);
		else
			SDL_GL_SetSwapInterval(0);

	}

	ImGui::SameLine();
	if (renderer3D->vsyncActive)ImGui::TextColored(ImVec4(1, 1, 0, 1), "On");
	else { ImGui::TextColored(ImVec4(1, 1, 0, 1), "Off"); }


}
void Application::DrawHardwareConsole() {

	ImGui::Text("SDL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "v2.0.12");

	ImGui::Separator();

	ImGui::Text("CPUs: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%d (Cache: %ikb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());

	ImGui::Text("System RAM: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "%iGb", SDL_GetSystemRAM() / 1000);

	ImGui::Text("Caps: ");
	ImGui::SameLine();

	if (SDL_Has3DNow() == SDL_TRUE) ImGui::TextColored(ImVec4(1, 1, 0, 1), "3DNow, ");
	ImGui::SameLine();
	if (SDL_HasAVX() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX, ");
	ImGui::SameLine();
	if (SDL_HasAVX2() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AVX2, ");
	ImGui::SameLine();
	if (SDL_HasAltiVec() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "AltiVec, ");
	ImGui::SameLine();
	if (SDL_HasMMX() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "MMX, ");
	
	if (SDL_HasRDTSC() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "RDTSC, ");
	ImGui::SameLine();
	if (SDL_HasSSE() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE, ");
	ImGui::SameLine();
	if (SDL_HasSSE2() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE2, ");
	ImGui::SameLine();
	if (SDL_HasSSE3() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE3, ");
	ImGui::SameLine();
	if (SDL_HasSSE41() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE41, ");
	ImGui::SameLine();
	if (SDL_HasSSE42() == SDL_TRUE)ImGui::TextColored(ImVec4(1, 1, 0, 1), "SSE42, ");
	
}