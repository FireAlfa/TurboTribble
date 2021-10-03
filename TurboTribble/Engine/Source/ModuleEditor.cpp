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
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	return ret;
}

update_status ModuleEditor::Update(float dt)
{
	// ImGui
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// Render your GUI
	ImGui::Begin("Demo window");
	ImGui::Button("Hello!");
	ImGui::End();

	ImGui::ShowDemoWindow();

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