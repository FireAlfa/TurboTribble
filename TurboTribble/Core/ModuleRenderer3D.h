#ifndef __MODULE_RENDERER_3D_H__
#define __MODULE_RENDERER_3D_H__

#include "Module.h"

#include "Globals.h"
#include "Light.h"
#include "SDL/include/SDL.h"

#define MAX_LIGHTS 8



class ModuleRenderer3D : public Module
{
public:

	// Constructor
	ModuleRenderer3D(Application* app, bool startEnabled = true);
	// Destructor
	~ModuleRenderer3D();

	// Initialize the renderer
	bool Init() override;
	// Set all drawing modes
	bool Start() override;
	// PreUpdate: clear buffer
	UpdateStatus PreUpdate(float dt) override;
	// PostUpdate present buffer to screen
	UpdateStatus PostUpdate(float dt) override;
	// Called before quitting, destroys the renderer
	bool CleanUp() override;


	// Control what happens when the window is resized
	void OnResize(int width, int height);

	// Draws Render Info
	void OnGui() override;

	// Load Renderer Info
	void OnLoad(const JSONReader& reader) override;
	// Save Renderer Info
	void OnSave(JSONWriter& writer) const override;

private:

	// Set specific drawing mode
	void DrawingModes(bool currentState, int glMode);

public:

	// ---- Renderer Variables -----
	
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	bool depthTestEnabled;
	bool cullFace;
	bool useLighting;
	bool useTexture;
	bool wireframeMode;
	bool vsyncActive;
	// -----------------------------

};

#endif // !__MODULE_RENDERER_3D_H__