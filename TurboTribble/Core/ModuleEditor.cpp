#include "ModuleEditor.h"

//Modules
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleImport.h"
#include "ModuleScene.h"
#include "ModuleViewportFrameBuffer.h"
#include "ModuleCamera3D.h"
#include "ModuleTextures.h"
#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "Globals.h"

//Tools
#include <string>
#include <stack>
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_internal.h"
#include "glew.h"
#include <gl/GL.h>



ModuleEditor::ModuleEditor(Application* app, bool startEnabled) : Module(app, startEnabled)
{
   
    showDemoWindow = false;
    showAnotherWindow = false;
    showAboutWindow = false;
    showConfWindow = true;

    showConsoleWindow = true;
    showHierarchyWindow = true;
    showInspectorWindow = true;
    showGameWindow = true;
    showSceneWindow = true;
    showTextures = true;

    currentColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    gameobjectSelected = nullptr;
}


// Destructor
ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
    TTLOG("+++++ Loading Editor Module +++++\n");
    return true;
}

// Called before render is available
bool ModuleEditor::Start()
{
	bool ret = true;
	
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    sceneWindow |= ImGuiWindowFlags_NoScrollbar;

    // Setup ImGui style by default
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->renderer3D->context);
    
    CreateGridBuffer();

    return ret;
}

UpdateStatus ModuleEditor::PreUpdate(float dt)
{

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(app->window->window);
    ImGui::NewFrame();

    return UpdateStatus::UPDATE_CONTINUE;

}

// PreUpdate: clear buffer
UpdateStatus ModuleEditor::Update(float dt)
{
    DrawGrid();
    // Creating MenuBar item as a root for docking windows
    if (DockingRootItem("Viewport", ImGuiWindowFlags_MenuBar)) {
        MenuBar();
        ImGui::End();
    }

    // Update status of each window and shows ImGui elements
    UpdateWindowStatus();

    return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleEditor::PostUpdate(float dt)
{

    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Rendering
    ImGui::Render();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
        SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
    }

    ImGui::EndFrame();

    return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleEditor::CleanUp()
{
    TTLOG("+++++ Quitting Editor Module +++++\n");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

	return true;
}

void ModuleEditor::CreateGridBuffer()
{
    std::vector<float3> vertices;
    std::vector<uint> indices;
    constexpr int slices = 200;
    constexpr float size = 2000.f;
    constexpr float halfSize = size * .5f;
    constexpr float sliceSize = size / static_cast<float>(slices);

   for (int i = 0; i < slices; ++i) 
   {
        const float x = -halfSize + static_cast<float>(i) * sliceSize;
        if (x > 0.01f || x < -0.01f)
        {
            vertices.push_back(float3(x, 0.f, -halfSize));
            vertices.push_back(float3(x, 0.f, halfSize));
            indices.push_back(vertices.size() - 2);
            indices.push_back(vertices.size() - 1);
        }
        const float z = -halfSize + static_cast<float>(i) * sliceSize;
        if (z > 0.01f || z < -0.01f)
        {
            vertices.push_back(float3(-halfSize, 0.f, z));
            vertices.push_back(float3(halfSize, 0.f, z));
            indices.push_back(vertices.size() - 2);
            indices.push_back(vertices.size() - 1);
        }
   }

    glGenVertexArrays(1, &grid.VAO);
    glBindVertexArray(grid.VAO);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float3), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    grid.length = (GLuint)indices.size() * 4;
}

void ModuleEditor::DrawGrid()
{
    GLboolean isLightingOn, isDepthTestOn;
    glGetBooleanv(GL_LIGHTING, &isLightingOn);
    glGetBooleanv(GL_DEPTH_TEST, &isDepthTestOn);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glBindVertexArray(grid.VAO);

    glColor3f(.5f, .5f, .5f);
    glDrawElements(GL_LINES, grid.length, GL_UNSIGNED_INT, NULL);

    glBindVertexArray(0);
    
    glBegin(GL_LINES);
        //x Axis
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(1000.f, 0.0f, 0.f);
        glColor3f(.2f, 0.f, 0.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(.2f, 0.f, 0.f);
        glVertex3f(-1000.f, 0.0f, 0.f);
        //z Axis
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.0f, 1000.f);
        glColor3f(0.f, 0.f, .2f);
        glVertex3f(0.f, 0.0f, 0.f);
        glColor3f(0.f, 0.f, .2f);
        glVertex3f(0.f, 0.0f, -1000.f);
    glEnd();

    isLightingOn ? glEnable(GL_LIGHTING) : 0;
    !isDepthTestOn ? glDisable(GL_DEPTH_TEST) : 0;

}

void ModuleEditor::AboutWindow()
{

    ImGui::Begin("About 3D Engine", &showAboutWindow);

    ImGui::Separator();
    ImGui::Text("3D Engine\n");
    ImGui::Separator();

    ImGui::Text("3rd Party Libraries used: ");
    ImGui::BulletText("SDL v2.0.12");
    ImGui::BulletText("Glew v2.1.0");
    ImGui::BulletText("OpenGL v3.1.0");
    ImGui::BulletText("ImGui v1.78");
    ImGui::BulletText("MathGeoLib v1.5");
    ImGui::BulletText("PhysFS v3.0.2");
    ImGui::BulletText("DevIL v1.7.8");
    ImGui::BulletText("Assimp v3.1.1");

    ImGui::Separator();
    ImGui::Text("LICENSE\n");
    ImGui::Separator();

    ImGui::Text("MIT License\n\n");
    ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy\n\nof this software and associated documentation files (the 'Software'), to deal\n");
    ImGui::Text("in the Software without restriction, including without limitation the rights\n\nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell\n");
    ImGui::Text("copies of the Software, and to permit persons to whom the Software is\n\nfurnished to do so, subject to the following conditions : \n");
    ImGui::Text("\n");
    ImGui::Text("The above copyright notice and this permission notice shall be included in all\n\ncopies or substantial portions of the Software.\n");
    ImGui::Text("\n");
    ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, \n");
    ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
    ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n");
    ImGui::Text("SOFTWARE.\n");

    //ImGui::Separator();


    ImGui::End();

}

void ModuleEditor::UpdateText(const char* text)
{
    consoleText.appendf(text);
}

bool ModuleEditor::DockingRootItem(char* id, ImGuiWindowFlags winFlags)
{
    // Setting windows as viewport size
    ImGuiViewport* viewport = ImGui::GetWindowViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    
    // Setting window style
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);

    // Viewport window flags just to have a non interactable white space where we can dock the rest of windows
    winFlags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar;
    winFlags |= ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground;

    bool temp = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    temp = ImGui::Begin(id, &temp, winFlags);
    ImGui::PopStyleVar(3);

    BeginDock(id, ImGuiDockNodeFlags_PassthruCentralNode);

    return temp;
}

void ModuleEditor::BeginDock(char* dockSpaceId, ImGuiDockNodeFlags dockFlags, ImVec2 size)
{
    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dock = ImGui::GetID(dockSpaceId);
        ImGui::DockSpace(dock, size, dockFlags);
    }
}

void ModuleEditor::MenuBar()
{

    /* ---- MAIN MENU BAR DOCKED ---- */
    if (ImGui::BeginMainMenuBar()) {

        /* ---- FILE ---- */
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl + S"))
            {
                // DO SOMETHING
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "(Alt+F4)")) app->closeEngine = true;
            ImGui::EndMenu();
        }

        /* ---- GAMEOBJECTS ---- */
        if (ImGui::BeginMenu("GameObject")) {

            if (ImGui::MenuItem("Create empty GameObject")) {
                app->scene->CreateGameObject();
            }

            if (ImGui::BeginMenu("3D Objects")) {
                if (ImGui::MenuItem("Cube")) {
                    GameObject* newGameObject = app->scene->CreateGameObject("Cube");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::CUBE);
                }
                if (ImGui::MenuItem("Sphere")) {
                    GameObject* newGameObject = app->scene->CreateGameObject("Sphere");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::SPHERE);
                }
                if (ImGui::MenuItem("Cylinder")) {
                    GameObject* newGameObject = app->scene->CreateGameObject("Cylinder");
                    ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::CYLINDER);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }


        /* ---- WINDOW ---- */
        if (ImGui::BeginMenu("Window")) {

            if (ImGui::MenuItem("Examples")) showDemoWindow = !showDemoWindow;
            ImGui::Separator();

            if (ImGui::BeginMenu("Workspace Style")) {
                if (ImGui::MenuItem("Dark")) 
                    ImGui::StyleColorsDark();
                if (ImGui::MenuItem("Classic")) 
                    ImGui::StyleColorsClassic();
                if (ImGui::MenuItem("Light")) 
                    ImGui::StyleColorsLight();
                if (ImGui::MenuItem("Custom")) 
                    ImGui::StyleColorsCustom();
                ImGui::EndMenu();
            }
            ImGui::Separator();

            if (ImGui::MenuItem("Hierarchy")) 
                showHierarchyWindow = !showHierarchyWindow;
            if (ImGui::MenuItem("Inspector")) 
                showInspectorWindow = !showInspectorWindow;
            if (ImGui::MenuItem("Scene")) 
                showSceneWindow = !showSceneWindow;
            if (ImGui::MenuItem("Game")) 
                showGameWindow = !showGameWindow;
            if (ImGui::MenuItem("Console")) 
                showConsoleWindow = !showConsoleWindow;
            if (ImGui::MenuItem("Textures")) 
                showTextures = !showTextures;

            ImGui::Separator();
            if (ImGui::MenuItem("Configuration")) 
                showConfWindow = !showConfWindow;
            

            ImGui::EndMenu();
        }

        /* ---- HELP ---- */
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) 
                showAboutWindow = !showAboutWindow;
            ImGui::EndMenu();
        }

    }

    ImGui::EndMainMenuBar();
}

void ModuleEditor::UpdateWindowStatus()
{

    // Demo
    if (showDemoWindow) 
        ImGui::ShowDemoWindow(&showDemoWindow);

    // About info
    if (showAboutWindow)  
        AboutWindow();

    // Config
    if (showConfWindow) {

        ImGui::Begin("Configuration", &showConfWindow);        
        app->OnGui();
        ImGui::End();

    }
    if (showTextures)
    {
        ImGui::Begin("Textures", &showTextures);
        for (auto& t : app->textures->textures)
        {
            ImGui::Image((ImTextureID)t.second.id, ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::SameLine();
            ImGui::PushID(t.second.id);
            if (ImGui::Button("Assign to selected"))
            {
                if (gameobjectSelected)
                {
                    ComponentMaterial* material = gameobjectSelected->GetComponent<ComponentMaterial>();
                    if (material)
                    {
                        material->SetTexture(t.second);
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::End();
    }
        
    // Console
    if (showConsoleWindow) {

        ImGui::Begin("Console", &showConsoleWindow);
        ImGui::TextUnformatted(consoleText.begin(), consoleText.end());
        ImGui::SetScrollHere(1.0f);
        ImGui::End();
    }

    // Inspector
    if (showInspectorWindow) {

        ImGui::Begin("Inspector", &showInspectorWindow);
        // Only shows info if any gameobject selected
        if (gameobjectSelected != nullptr) 
            InspectorGameObject(); 

        ImGui::End();

    }

    // Hierarchy
    if (showHierarchyWindow) {


        ImGui::Begin("Hierarchy", &showHierarchyWindow);

        // Just cleaning gameObjects(not textures,buffers...)
        if (ImGui::Button("Clear", { 60,20 })) 
        {
            app->editor->gameobjectSelected = nullptr;
            app->scene->CleanUp(); //Clean GameObjects 
        }
        ImGui::SameLine();
        if (ImGui::Button("New", { 60,20 }))
        {
            app->scene->CreateGameObject();
        }
        std::stack<GameObject*> s;
        std::stack<uint> indents;
        s.push(app->scene->root);
        indents.push(0);
        while (!s.empty())
        {
            GameObject* go = s.top();
            uint indentsAmount = indents.top();
            s.pop();
            indents.pop();

            ImGuiTreeNodeFlags nodeFlags = 0;
            if (go->isSelected)
                nodeFlags |= ImGuiTreeNodeFlags_Selected;
            if (go->children.size() == 0)
                nodeFlags |= ImGuiTreeNodeFlags_Leaf; 
            for (uint i = 0; i < indentsAmount; ++i)
            {
                ImGui::Indent();
            }

            if (ImGui::TreeNodeEx(go->name.c_str(), nodeFlags)) 
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("DragDropHierarchy", &go, sizeof(GameObject*), ImGuiCond_Once);
                    ImGui::Text("%s", go->name.c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
                    {
                        IM_ASSERT(payload->DataSize == sizeof(GameObject*));
                        GameObject* droppedGo = (GameObject*)*(const int*)payload->Data;
                        if (droppedGo)
                        {
                            droppedGo->parent->RemoveChild(droppedGo);
                            go->AttachChild(droppedGo);
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::IsItemClicked()) {
                    gameobjectSelected ? gameobjectSelected->isSelected = !gameobjectSelected->isSelected : 0;
                    gameobjectSelected = go;
                    gameobjectSelected->isSelected = !gameobjectSelected->isSelected;
                    if (gameobjectSelected->isSelected)
                    {
                        TTLOG("+++ GameObject selected name: %s +++\n", gameobjectSelected->name.c_str());
                    }
                    else
                    {
                        TTLOG("+++ GameObject unselected name: %s +++\n", gameobjectSelected->name.c_str());
                    }
                }
                for (GameObject* child : go->children)
                {
                    s.push(child);
                    indents.push(indentsAmount + 1);
                }

                for (uint i = 0; i < indentsAmount; ++i)
                {
                    ImGui::Unindent();
                }

                ImGui::TreePop();
            }
        }
        ImGui::End();
    }

    if (showGameWindow) {
        ImGui::Begin("Game", &showGameWindow, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
        ImGui::End();
    }

    if (showSceneWindow) {

        ImGui::Begin("Scene", &showSceneWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImVec2 viewPortSize = ImGui::GetCurrentWindow()->Size;
        ImVec2 viewPortRegion = ImVec2(ImGui::GetWindowContentRegionMax().x - 10, ImGui::GetWindowContentRegionMax().y - 30);
        if (viewPortSize.x != lastViewportSize.x || viewPortSize.y != lastViewportSize.y)
        {
            lastViewportSize = viewPortSize;
            app->camera->aspectRatio = viewPortRegion.x / viewPortRegion.y;
            app->camera->RecalculateProjection();
        }
        ImGui::Image((ImTextureID)app->viewportBuffer->texture, viewPortRegion, ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
}

void ModuleEditor::InspectorGameObject() 
{
    if (gameobjectSelected)
        gameobjectSelected->OnGui();
}

ModuleEditor::Grid::~Grid()
{
    glDeleteBuffers(1, &VAO);
}