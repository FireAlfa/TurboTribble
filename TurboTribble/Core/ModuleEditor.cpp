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
#include "ImGui/imgui.h"
#include "glew.h"
#include <gl/GL.h>
#include "Assimp/include/version.h"



ModuleEditor::ModuleEditor(Application* app, bool startEnabled) : Module(app, startEnabled)
{
    // ----- Flags set -----
    
    showDemoWindow = false;
    showAnotherWindow = false;
    showAboutWindow = false;
    showConfigWindow = true;
    showInspectorWindow = true;
    showHierarchyWindow = true;
    showSceneWindow = true;
    showGameWindow = true;
    showTexturesWindow = true;
    showConsoleWindow = true;

    darkStyle = true;
    classicStyle = false;
    lightStyle = false;
    customStyle = false;
    // ---------------------

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
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    sceneWindow |= ImGuiWindowFlags_NoScrollbar;

    // Setup ImGui style by default
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
	ImGui_ImplOpenGL3_Init();
    ImGui_ImplSDL2_InitForOpenGL(app->window->window, app->renderer3D->context);
    
    CreateGridBuffer();


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

UpdateStatus ModuleEditor::PreUpdate(float dt)
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(app->window->window);
    ImGui::NewFrame();

    // Check if any Keyboard shortcut has been used
    CheckKeyboardInputs();

    return UpdateStatus::UPDATE_CONTINUE;
}

// PreUpdate: clear buffer
UpdateStatus ModuleEditor::Update(float dt)
{
    DrawGrid();
    // Creating MenuBar item as a root for docking windows
    if (DockingRootItem("Viewport", ImGuiWindowFlags_MenuBar))
    {
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

/* ----- BACKGROUND GRID ----- */
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
ModuleEditor::Grid::~Grid()
{
    glDeleteBuffers(1, &VAO);
}

// Add text to the console Log
void ModuleEditor::UpdateText(const char* text)
{
    consoleText.appendf(text);
}

/* ----- DOCKING ----- */
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
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dock = ImGui::GetID(dockSpaceId);
        ImGui::DockSpace(dock, size, dockFlags);
    }
}

/* ----- MENU BAR ----- */
void ModuleEditor::MenuBar()
{
    // Main Menu Bar
    if (ImGui::BeginMainMenuBar())
    {

        // File Tab
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save", "Ctrl + S"))
            {
                // DO SOMETHING
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) app->closeEngine = true;
            ImGui::EndMenu();
        }

        // Edit Tab
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Frame Selected", "F"))
            {
                app->camera->FrameSelected();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Configuration", "Ctrl+Shift+O", showConfigWindow))
            {
                showConfigWindow = !showConfigWindow;
            }
            ImGui::EndMenu();
        }

        // GameObjects Tab
        if (ImGui::BeginMenu("GameObject"))
        {

            if (ImGui::MenuItem("Create Empty", "Ctrl+Shift+N"))
            {
                app->scene->CreateGameObject();
            }

            if (ImGui::BeginMenu("3D Objects"))
            {
                PremadeObjectsMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // Window Tab
        if (ImGui::BeginMenu("Window"))
        {

            if (ImGui::BeginMenu("General"))
            {
                if (ImGui::MenuItem("Scene", "Ctrl+1", showSceneWindow))
                    showSceneWindow = !showSceneWindow;
                if (ImGui::MenuItem("Game", "Ctrl+2", showGameWindow))
                    showGameWindow = !showGameWindow;
                if (ImGui::MenuItem("Inspector", "Ctrl+3", showInspectorWindow))
                    showInspectorWindow = !showInspectorWindow;
                if (ImGui::MenuItem("Hierarchy", "Ctrl+4", showHierarchyWindow))
                    showHierarchyWindow = !showHierarchyWindow;
                if (ImGui::MenuItem("Project", "Ctrl+5", false, false)) {}
                if (ImGui::MenuItem("Textures", "Ctrl+6", showTexturesWindow))
                    showTexturesWindow = !showTexturesWindow;
                if (ImGui::MenuItem("Console", "Ctrl+Shift+C", showConsoleWindow))
                    showConsoleWindow = !showConsoleWindow;
                
                ImGui::EndMenu();
            }

            ImGui::Separator();

            if (ImGui::BeginMenu("Workspace Style"))
            {
                if (ImGui::MenuItem("Dark", "", darkStyle))
                {
                    ImGui::StyleColorsDark();
                    if (darkStyle == false)
                        darkStyle = true;
                    classicStyle = false;
                    lightStyle = false;
                    customStyle = false;
                }
                if (ImGui::MenuItem("Classic", "", classicStyle))
                {
                    ImGui::StyleColorsClassic();
                    if (classicStyle == false)
                        classicStyle = true;
                    darkStyle = false;
                    lightStyle = false;
                    customStyle = false;
                }
                if (ImGui::MenuItem("Light", "", lightStyle))
                {
                    ImGui::StyleColorsLight();
                    if (lightStyle == false)
                        lightStyle = true;
                    darkStyle = false;
                    classicStyle = false;
                    customStyle = false;
                }
                if (ImGui::MenuItem("Custom", "", customStyle))
                {
                    ImGui::StyleColorsCustom();
                    if (customStyle == false)
                        customStyle = true;
                    darkStyle = false;
                    classicStyle = false;
                    lightStyle = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        // Help Tab
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About TurboTribble")) { showAboutWindow = !showAboutWindow, SW_SHOW; }
            ImGui::Separator();
            if (ImGui::MenuItem("Documentation")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/wiki", 0, 0, SW_SHOW); }
            if (ImGui::MenuItem("Download Latest")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/releases", 0, 0, SW_SHOW); }
            if (ImGui::MenuItem("Report a Bug")) { ShellExecute(0, 0, "https://github.com/FireAlfa/TurboTribble/issues", 0, 0, SW_SHOW); }
            ImGui::Separator();
            if (ImGui::MenuItem("Gui Demo", 0, showDemoWindow)) { showDemoWindow = !showDemoWindow; }

            ImGui::EndMenu();
        }
    }

    ImGui::EndMainMenuBar();
}

// List with the premade 3D Objects, used to be inserted in Menus
void ModuleEditor::PremadeObjectsMenu()
{
    if (ImGui::MenuItem("Cube"))
    {
        GameObject* newGameObject = app->scene->CreateGameObject("Cube");
        ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::CUBE);
    }
    if (ImGui::MenuItem("Sphere"))
    {
        GameObject* newGameObject = app->scene->CreateGameObject("Sphere");
        ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::SPHERE);
    }
    if (ImGui::MenuItem("Cylinder"))
    {
        GameObject* newGameObject = app->scene->CreateGameObject("Cylinder");
        ComponentMesh* newMesh = new ComponentMesh(newGameObject, ComponentMesh::Shape::CYLINDER);
    }
}

/* ----- CHECK IF EACH WINDOW SHOULD BE DRAWN ----- */
void ModuleEditor::UpdateWindowStatus()
{
    // Demo
    if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);

    // About info
    if (showAboutWindow) AboutWindow();

    // Config
    if (showConfigWindow) ConfigurationWindow();

    // Textures
    if (showTexturesWindow) TexturesWindow();
        
    // Console
    if (showConsoleWindow) ConsoleWindow();

    // Inspector
    if (showInspectorWindow) InspectorWindow();

    // Hierarchy
    if (showHierarchyWindow) HierarchyWindow();

    // Game
    if (showGameWindow) GameWindow();
    
    // Scene
    if (showSceneWindow) SceneWindow();
}

/* ---------- ABOUT WINDOW ---------- */
void ModuleEditor::AboutWindow()
{

    ImGui::Begin("About TurboTribble Engine", &showAboutWindow);
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

/* ---------- CONFIGURATION WINDOW ---------- */
void ModuleEditor::ConfigurationWindow()
{
    ImGui::Begin("Configuration", &showConfigWindow);
    app->OnGui();
    ImGui::End();
}

/* ---------- TEXTURES WINDOW ---------- */
void ModuleEditor::TexturesWindow()
{
    ImGui::Begin("Textures", &showTexturesWindow);
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

/* ---------- CONSOLE WINDOW ---------- */
void ModuleEditor::ConsoleWindow()
{
    ImGui::Begin("Console", &showConsoleWindow);
    ImGui::TextUnformatted(consoleText.begin(), consoleText.end());
    //ImGui::SetScrollHere(1.0f);
    ImGui::End();
}

/* ---------- INSPECTOR WINDOW ---------- */
void ModuleEditor::InspectorWindow()
{
    ImGui::Begin("Inspector", &showInspectorWindow);
    // Only shows info if any gameobject selected
    if (gameobjectSelected != nullptr)
        InspectorGameObject();



    ImGui::End();
}
// Inspector info for the selected GameObject
void ModuleEditor::InspectorGameObject()
{
    if (gameobjectSelected)
        gameobjectSelected->OnGui();
}

/* ---------- HIERARCHY WINDOW ---------- */
void ModuleEditor::HierarchyWindow()
{
    ImGui::Begin("Hierarchy", &showHierarchyWindow);

    // Just cleaning gameObjects(not textures,buffers...)
    //if (ImGui::Button("Clear", { 60,20 }))
    //{
    //    app->editor->gameobjectSelected = nullptr;
    //    app->scene->CleanUp(); //Clean GameObjects
    //}
    //ImGui::SameLine();

    // "+" Button with a drop down to add GameObjects
    ImGui::SetNextItemWidth(35.f);
    if (ImGui::BeginCombo(" ", "+", ImGuiComboFlags_PopupAlignLeft))
    {
        if (ImGui::Selectable("Create Empty"))
            app->scene->CreateGameObject(app->scene->root);

        if (ImGui::Selectable("Create Empty Child"))
            app->scene->CreateGameObject(gameobjectSelected);

        ImGui::Separator();

        if (ImGui::BeginMenu("3D Objects"))
        {
            PremadeObjectsMenu();
            ImGui::EndMenu();
        }

        ImGui::EndCombo();
    }

    // Right click menu
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::MenuItem("Create Empty"))
            app->scene->CreateGameObject();

        if (gameobjectSelected != nullptr)
        {
            if (ImGui::MenuItem("Create Empty Child"))
                app->scene->CreateGameObject(gameobjectSelected);

            ImGui::Separator();

            // Delete GameObject
            if (ImGui::MenuItem("Delete"))
            {
                gameobjectSelected->Delete();
                RELEASE(gameobjectSelected);
            }
        }

        ImGui::EndPopup();
    }

    // Deselect GameObject by clicking outside of the list
    if (((ImGui::IsMouseClicked(ImGuiMouseButton_Left)) || (ImGui::IsMouseClicked(ImGuiMouseButton_Right))) && gameobjectSelected != nullptr && ImGui::IsWindowHovered() && gameobjectSelected->isSelected)
    {
        gameobjectSelected->isSelected = !gameobjectSelected->isSelected;
        gameobjectSelected = nullptr;
    }

    // Reparenting and selecting
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

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;
        if (go->isSelected)
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        if (go->children.size() == 0)
            nodeFlags |= ImGuiTreeNodeFlags_Leaf;
        else
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;

        for (uint i = 0; i < indentsAmount; ++i)
        {
            ImGui::Indent();
        }

        if (strcmp(go->name.c_str(), "Root") == 0) // Root is open by default so you can see its children
        {
            nodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        }
        if (ImGui::TreeNodeEx(go->name.c_str(), nodeFlags))
        {
            if (strcmp(go->name.c_str(), "Root") != 0) // Root can not change parent
            {
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("DragDropHierarchy", &go, sizeof(GameObject*), ImGuiCond_Once);
                    ImGui::Text("%s", go->name.c_str());
                    ImGui::EndDragDropSource();
                }
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

            if ((ImGui::IsItemClicked(ImGuiMouseButton_Left)) || (ImGui::IsItemClicked(ImGuiMouseButton_Right)))
            {
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

/* ---------- GAME WINDOW ---------- */
void ModuleEditor::GameWindow()
{
    ImGui::Begin("Game", &showGameWindow, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar);
    ImGui::End();
}

/* ---------- SCENE WINDOW ---------- */
void ModuleEditor::SceneWindow()
{
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

    ImVec2 posW = ImGui::GetWindowPos();
    if (ImGui::IsWindowHovered())
    {
        app->camera->sceneHovered = true;
    }
    else
    {
        app->camera->sceneHovered = false;
    }
    
    ImGui::End();
}

// Do actions when Shortcuts are executed
void ModuleEditor::CheckKeyboardInputs()
{
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_1) == KeyState::KEY_DOWN))
    {
        showSceneWindow = !showSceneWindow;
    }
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_2) == KeyState::KEY_DOWN))
    {
        showGameWindow = !showGameWindow;
    }
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_3) == KeyState::KEY_DOWN))
    {
        showInspectorWindow = !showInspectorWindow;
    }
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_4) == KeyState::KEY_DOWN))
    {
        showHierarchyWindow = !showHierarchyWindow;
    }
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_5) == KeyState::KEY_DOWN))
    {
        //showProjectWindow = !showProjectWindow;
    }
    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_6) == KeyState::KEY_DOWN))
    {
        showTexturesWindow = !showTexturesWindow;
    }

    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_DOWN ||
            app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_C) == KeyState::KEY_DOWN))
    {
        showConsoleWindow = !showConsoleWindow;
    }

    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_DOWN ||
            app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_O) == KeyState::KEY_DOWN))
    {
        showConfigWindow = !showConfigWindow;
    }

    if ((app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_DOWN ||
        app->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_DOWN ||
            app->input->GetKey(SDL_SCANCODE_LSHIFT) == KeyState::KEY_REPEAT) &&
        (app->input->GetKey(SDL_SCANCODE_N) == KeyState::KEY_DOWN))
    {
        app->scene->CreateGameObject();
    }

    if (app->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN)
    {
        if (gameobjectSelected != nullptr)
        {
            if (strcmp(gameobjectSelected->name.c_str(), "Root") != 0)
            {
                gameobjectSelected->Delete();
                RELEASE(gameobjectSelected);
            }
        }
    }
}