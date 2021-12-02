#include "ModuleRenderer3D.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"

#include "Globals.h"

#include "glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>



ModuleRenderer3D::ModuleRenderer3D(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	vsyncActive = true;

	depthTestEnabled = true;
	cullFace = true;
	useLighting = true;
	useTexture = true;
	wireframeMode = false;
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	TTLOG("+++++ Loading Renderer Module +++++\n");
	bool ret = true;
		
	// Create context
	context = SDL_GL_CreateContext(app->window->window);
	if(context == NULL)
	{
		TTLOG("##### OpenGL context could not be created! SDL_Error: %s #####\n", SDL_GetError());
		ret = false;
	}

	GLenum err = glewInit();

	TTLOG("+++ Using Glew %s +++\n", glewGetString(GLEW_VERSION));
	TTLOG("+++ Vendor: %s +++\n", glGetString(GL_VENDOR));
	TTLOG("+++ Renderer: %s +++\n", glGetString(GL_RENDERER));
	TTLOG("+++ OpenGL version supported %s +++\n", glGetString(GL_VERSION));
	TTLOG("+++ GLSL: %s +++\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	

	if(ret == true)
	{
		// Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			TTLOG("##### Warning: Unable to set VSync! SDL Error: %s #####\n", SDL_GetError());

		// Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}

		// Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		// Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		// Initialize BlendFunc
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			TTLOG("##### Error initializing OpenGL! %s #####\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat lightModelAmbient[] = {0.3f, 0.3f, 0.3f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.9f, 0.9f, 0.9f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		lights[0].Active(true);

		
		GLfloat materialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);

		GLfloat materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

bool ModuleRenderer3D::Start()
{
	DrawingModes(depthTestEnabled, GL_DEPTH_TEST);

	DrawingModes(cullFace, GL_CULL_FACE);

	DrawingModes(useLighting, GL_LIGHTING);

	DrawingModes(useTexture, GL_TEXTURE_2D);

	wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return true;
}

// PreUpdate: clear buffer
UpdateStatus ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Recalculate matrix -------------
	app->camera->CalculateViewMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(app->camera->cameraFrustum.ProjectionMatrix().Transposed().ptr());
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(app->camera->viewMatrix.Transposed().ptr());

	// light 0 on cam pos
	lights[0].SetPos(app->camera->position.x, app->camera->position.y, app->camera->position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UpdateStatus::UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
UpdateStatus ModuleRenderer3D::PostUpdate(float dt)
{
	SDL_GL_SwapWindow(app->window->window);

	return UpdateStatus::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	TTLOG("+++++ Quitting Renderer Module +++++\n");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	app->camera->RecalculateProjection();
}

void ModuleRenderer3D::OnGui()
{
	if (ImGui::CollapsingHeader("Render"))
	{
		ImGui::TextUnformatted("Render Options");
		if (ImGui::Checkbox("Depth Test", &depthTestEnabled))
			DrawingModes(depthTestEnabled, GL_DEPTH_TEST);

		if (ImGui::Checkbox("Cull Face", &cullFace))
			DrawingModes(cullFace, GL_CULL_FACE);

		if (ImGui::Checkbox("Lighting ON/OFF", &useLighting))
			DrawingModes(useLighting, GL_LIGHTING);

		if (ImGui::Checkbox("Texture Draw", &useTexture))
			DrawingModes(useTexture, GL_TEXTURE_2D);

		if (ImGui::Checkbox("Wireframe Mode", &wireframeMode)) {
			wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}
}

void ModuleRenderer3D::OnLoad(const JSONReader& reader)
{
	if (reader.HasMember("render"))
	{
		const auto& config = reader["render"];
		LOAD_JSON_BOOL(depthTestEnabled)
		LOAD_JSON_BOOL(cullFace)
		LOAD_JSON_BOOL(useTexture)
		LOAD_JSON_BOOL(wireframeMode)
		LOAD_JSON_BOOL(vsyncActive)
	}
}

void ModuleRenderer3D::OnSave(JSONWriter& writer) const
{
	writer.String("render");
	writer.StartObject();
	SAVE_JSON_BOOL(depthTestEnabled)
	SAVE_JSON_BOOL(cullFace)
	SAVE_JSON_BOOL(useLighting)
	SAVE_JSON_BOOL(useTexture)
	SAVE_JSON_BOOL(wireframeMode)
	SAVE_JSON_BOOL(vsyncActive)
	writer.EndObject();
}

void ModuleRenderer3D::DrawingModes(bool currentState, int glMode)
{
	currentState ? glEnable(glMode) : glDisable(glMode);
}