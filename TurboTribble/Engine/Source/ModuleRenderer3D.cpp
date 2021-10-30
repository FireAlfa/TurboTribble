#include "ModuleRenderer3D.h"
#include "Application.h"

#include "glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleSceneIntro.h"
#include "ModuleEditor.h"



// Constructor
ModuleRenderer3D::ModuleRenderer3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{

}
// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	TTLOG("+++++ Init Module 3DRenderer +++++");
	bool ret = true;
	

	// Use OpenGL 3.1 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);


	//Create context
	glContext = SDL_GL_CreateContext(app->window->window);
	if(glContext == NULL)
	{
		TTLOG("##### OpenGL context could not be created! SDL_Error: %s #####\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		// Current OpenGL Context
		SDL_GL_MakeCurrent(app->window->window, glContext);

		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			TTLOG("##### Warning: Unable to set VSync! SDL Error: %s #####\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}

		// Check errors Glew
		GLenum err = glewInit();
		
		// Glew version
		TTLOG("+++ Using Glew %s +++", glewGetString(GLEW_VERSION));
		

		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);

		TTLOG("+++ Vendor: %s +++", glGetString(GL_VENDOR));
		TTLOG("+++ Renderer: %s +++", glGetString(GL_RENDERER));
		TTLOG("+++ OpenGL version supported: %s +++", glGetString(GL_VERSION));
		TTLOG("+++ GLSL: %s +++\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}
	
// Clear the buffer
UpdateStatus ModuleRenderer3D::PreUpdate(float dt)
{
	// Get background color
	Color c = app->camera->backgroundColor;
	glClearColor(c.r, c.g, c.b, c.a);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(app->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(app->camera->position.x, app->camera->position.y, app->camera->position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UpdateStatus::UPDATE_CONTINUE;
}

// Present the buffer to screen, calls the draws
UpdateStatus ModuleRenderer3D::PostUpdate(float dt)
{
	// Draw the Scene
	app->scene_intro->Draw();

	// TODO Debug Draw
	/*if (debug_draw == true)
	{
		BeginDebugDraw();
		App->DebugDraw();
		EndDebugDraw();
	}*/

	// Draw the Editor
	app->editor->Draw();

	SDL_GL_SwapWindow(app->window->window);
	return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting, destroys the renderer
bool ModuleRenderer3D::CleanUp()
{
	TTLOG("+++++ Quitting Module 3DRenderer +++++");

	SDL_GL_DeleteContext(glContext);

	return true;
}


// Control what happens when the window is resized
void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	projectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&projectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
