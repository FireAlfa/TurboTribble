#include "ModuleScene.h"

#include "Application.h"
#include "ModuleImport.h"
#include "ModuleTextures.h"
#include "ModuleEditorCamera.h"
#include "ModuleEditor.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleViewportFrameBuffer.h"

#include "Globals.h"

#include "glew.h"
#include "ImGui/imgui.h"
#include <stack>
#include <queue>



ModuleScene::ModuleScene(Application* app, bool startEnabled) : Module(app, startEnabled)
{
}

bool ModuleScene::Init()
{
	TTLOG("+++++ Loading Scene Module +++++\n");
	return true;
}

bool ModuleScene::Start()
{
	bool ret = true;
	
	root = new GameObject("Root");

	mainCamera = CreateGameObject("Main Camera", root);
	mainCamera->CreateComponent<ComponentCamera>();
	mainCamera->transform->SetPosition(float3(0.0f, 5.0f, -15.0f));
	mainCamera->GetComponent<ComponentCamera>()->LookAt(float3::zero);

	// Loading house and textures since beginning
	app->import->LoadGeometry("Assets/Models/street.fbx");
	
	return ret;
}

bool ModuleScene::CleanUp()
{
	TTLOG("+++++ Quitting Scene Module +++++\n");

	std::stack<GameObject*> s;
	for (GameObject* child : root->children)	
	{
		s.push(child);
	}
	root->children.clear();

	while (!s.empty())
	{
		GameObject* go = s.top();
		s.pop();
		for (GameObject* child : go->children)
		{
			s.push(child);
		}
		go->children.clear();
		delete go;
	}

	delete root;

	return true;
}

UpdateStatus ModuleScene::Update(float dt)
{
	// ----- Update Stuff on Scene Window -----
	
	app->sceneViewportBuffer->PreUpdate(dt);
	std::queue<GameObject*> s;
	for (GameObject* child : root->children)
	{
		s.push(child);
	}

	while (!s.empty())
	{
		GameObject* go = s.front();
		go->Update(dt);
		s.pop();
		for (GameObject* child : go->children)
		{
			s.push(child);
		}
	}

	app->editor->DrawGrid();
	app->sceneViewportBuffer->PostUpdate(dt);
	// ----------------------------------------


	// ----- Update Stuff on Game Window -----
	
	if (mainCamera != nullptr)
	{
		mainCamera->GetComponent<ComponentCamera>()->PreUpdate(dt);

		std::queue<GameObject*> g;
		for (GameObject* child : root->children)
		{
			g.push(child);
		}

		while (!g.empty())
		{
			GameObject* go = g.front();
			go->Update(dt);
			g.pop();
			for (GameObject* child : go->children)
			{
				g.push(child);
			}
		}

		app->gameViewportBuffer->PostUpdate(dt);
	}
	// ---------------------------------------

	glDisable(GL_DEPTH_TEST);

	if (app->editor->gameobjectSelected)
	{
		ComponentTransform* transform = app->editor->gameobjectSelected->GetComponent<ComponentTransform>();
		float3 pos = transform->GetPosition();
		glLineWidth(10.f);
		glBegin(GL_LINES);
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Right().x, pos.y + transform->Right().y, pos.z + transform->Right().z);
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Front().x, pos.y + transform->Front().y, pos.z + transform->Front().z);
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(pos.x, pos.y, pos.z);
		glVertex3f(pos.x + transform->Up().x, pos.y + transform->Up().y, pos.z + transform->Up().z);
		glEnd();
		glLineWidth(1.f);
	}

	glEnable(GL_DEPTH_TEST);

	return UpdateStatus::UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{

	GameObject* temp = new GameObject();
	if (parent)
		parent->AttachChild(temp);
	else
		root->AttachChild(temp);
	return temp;
}
GameObject* ModuleScene::CreateGameObject(const std::string name, GameObject* parent)
{
	GameObject* temp = new GameObject(name);
	if (parent)
		parent->AttachChild(temp);
	else
		root->AttachChild(temp);
	return temp;
}