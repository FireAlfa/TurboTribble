#ifndef __MODULEEDITOR_H__
#define __MODULEEDITOR_H__

#include "Module.h"

#include <vector>
#include <gl/GL.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include "SDL/include/SDL_opengl.h"



class ModuleEditor : public Module
{
public:
	ModuleEditor(Application* app, bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	ImGuiIO io;
};
#endif // !__MODULEEDITOR_H__