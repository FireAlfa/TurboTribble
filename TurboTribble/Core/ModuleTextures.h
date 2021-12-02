#ifndef __MODULE_TEXTURES_H__
#define __MODULE_TEXTURES_H__

#include "Module.h"

#include <map>
#include <string>



struct TextureObject
{
	TextureObject() = default;

	TextureObject(std::string name, uint id, uint width, uint height) : name(name), id(id), width(width), height(height) {};
	std::string name;
	uint id = 0;
	uint width = 0, height = 0;
};


class ModuleTextures : public Module
{
public:

	// Constructor
	ModuleTextures(Application* app, bool startEnabled = true);

	// Init module
	bool Init() override;
	// Load the Texture Loader
	bool Start() override;
	// Called before quitting
	bool CleanUp() override;


	// Load new texture from file path
	const TextureObject& Load(const std::string& path, bool useMipMaps = false);
	// Get texture from path
	const TextureObject& Get(const std::string& path);
	// Find texture in path
	bool Find(const std::string& path) const;

public:

	// ----- Texture Variables -----
	
	uint32 whiteFallback = 0, blackFallback = 0, checkers = 0;
	std::map<const std::string, TextureObject> textures;
	// -----------------------------

};

#endif // !__MODULE_TEXTURES_H__