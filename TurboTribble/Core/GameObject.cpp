#include "GameObject.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"



GameObject::GameObject()
{
	name = name + ("GameObject");
	parent = nullptr;

	transform = CreateComponent<ComponentTransform>();

	active = true;
}

GameObject::GameObject(const std::string name) : name(name) 
{
	transform = CreateComponent<ComponentTransform>();

	active = true;

	TTLOG("~~~~ Creating GameObject with name %s ~~~~\n", this->name.c_str());
}


GameObject::~GameObject()
{

	for (size_t i = 0; i < components.size(); i++)
	{
		RELEASE(components[i]);
	}

	components.clear();

	for (GameObject* go : children)
	{
		RELEASE(go);
	}

	parent = nullptr;
}

void GameObject::Update(float dt) 
{
	for (Component* component : components)
	{
		component->Update(dt);
		component->Draw();

		if (app->editor->gameobjectSelected == this)
		{
			component->DrawDebug();
		}
	}
}

void GameObject::OnGui()
{
	if (app->scene->root != this)
	{
		ImGui::TextColored(ImVec4(1, 1, 0, 1), "%s", name.c_str());
		if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Select all text when you enter the box and only modify it when Enter is pressed
			ImGuiInputTextFlags textFlags = ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue;
			char text[32];
			// Name box
			strcpy_s(text, name.c_str());
			if (ImGui::InputText("Name", text, IM_ARRAYSIZE(text), textFlags))
			{
				name = text;
			}
		}
		ImGui::Separator();

		for (Component* component : components)
		{
			component->OnGui();
		}

		ImGui::Separator();

		ImGuiComboFlags addComponentFlags = ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_PopupAlignLeft;
		float buttonWidth = ImGui::CalcTextSize("Add Component").x + 8;
		ImGui::SetNextItemWidth(buttonWidth);
		ImGui::SetCursorPosX(ImGui::GetWindowWidth()/2 - buttonWidth / 2);
		if (ImGui::BeginCombo(" ", "Add Component", addComponentFlags))
		{
			if (ImGui::Selectable("Mesh"))
			{
				/*ComponentMesh* mesh = CreateComponent<ComponentMesh>();
				AddComponent(mesh);*/
			}
			if (ImGui::Selectable("Texture"))
			{

			}

			ImGui::EndCombo();
		}
	}
}

void GameObject::Delete()
{
	TTLOG("---- Destroying GameObject with name %s ----\n", this->name.c_str());

	while (components.size() != 0)
		DeleteComponent(components.at(0));
	while (children.size() != 0)
		RemoveChild(children.at(0));

	parent->RemoveChild(this);
}

void GameObject::DeleteComponent(Component* component)
{

	auto componentIt = std::find(components.begin(), components.end(), component);
	if (componentIt != components.end())
	{
		components.erase(componentIt);
		components.shrink_to_fit();
	}
}

void GameObject::AddComponent(Component* component)
{
	components.push_back(component);
}

void GameObject::AttachChild(GameObject* child)
{
	child->parent = this;
	children.push_back(child);
	child->transform->NewAttachment();
	child->PropagateTransform();
}

void GameObject::RemoveChild(GameObject* child)
{
	auto it = std::find(children.begin(), children.end(), child);
	if (it != children.end())
	{
		children.erase(it);
	}
}

void GameObject::PropagateTransform()
{
	for (GameObject* go : children)
	{
		go->transform->OnParentMoved();
	}
}