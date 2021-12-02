#include "ModuleTextures.h"

#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleEditor.h"

#include "Globals.h"

#include "glew.h"
// DevIL Image Library
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64



ModuleTextures::ModuleTextures(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	ilInit();
	iluInit();
	ilutInit();
}

bool ModuleTextures::Init()
{
	TTLOG("+++++ Loading Textures Module +++++\n");
	return true;
}

bool ModuleTextures::Start()
{
	GLubyte fallbackImageWhite[4] = { GLubyte(255), GLubyte(255), GLubyte(255), GLubyte(255) };

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &whiteFallback);
	glBindTexture(GL_TEXTURE_2D, whiteFallback);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackImageWhite);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLubyte fallbackImageBlack[4] = { GLubyte(0), GLubyte(0), GLubyte(0), GLubyte(0) };

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &blackFallback);
	glBindTexture(GL_TEXTURE_2D, blackFallback);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, fallbackImageBlack);
	glBindTexture(GL_TEXTURE_2D, 0);

	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_HEIGHT][4];

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkers);
	glBindTexture(GL_TEXTURE_2D, checkers);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage[0]);
	glBindTexture(GL_TEXTURE_2D, 0);


	if (blackFallback != 0u && whiteFallback != 0u && checkers != 0u)
	{
		textures.insert(std::make_pair("BLACK_FALLBACK", TextureObject("BLACK_FALLBACK", static_cast<uint>(blackFallback), 1, 1)));
		textures.insert(std::make_pair("WHITE_BALLBACK", TextureObject("WHITE_BALLBACK", static_cast<uint>(whiteFallback), 1, 1)));
		textures.insert(std::make_pair("CHECKERS", TextureObject("CHECKERS", static_cast<uint>(checkers), CHECKERS_WIDTH, CHECKERS_HEIGHT)));
		return true;
	}

	return false;
}

// Called before quitting
bool ModuleTextures::CleanUp() // can be called to reset stored textures
{
	TTLOG("+++++ Quitting Textures Module +++++\n");
	
	for (auto& t : textures)
		glDeleteTextures(1, &t.second.id);
	
	textures.clear();
	return true;
}

// Load new texture from file path
const TextureObject& ModuleTextures::Load(const std::string& path, bool useMipMaps)
{
	TTLOG("+++ Loading texture -> %s +++\n", path.c_str());

	ILuint imageId;
	ilGenImages(1, &imageId);
	ilBindImage(imageId);

	char* data;
	uint bytes = app->fileSystem->Load(path.c_str(), &data);

	if (bytes != 0)
	{
		if (ilLoadL(IL_TYPE_UNKNOWN, data, bytes))
		{
			GLuint textureId = 0;
			glBindTexture(GL_TEXTURE_2D, 0);
			glGenTextures(1, &textureId);

			glBindTexture(GL_TEXTURE_2D, textureId);

			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			int channels = ilGetInteger(IL_IMAGE_CHANNELS);
			if (channels == 3)
			{
				ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
			}
			else if (channels == 4)
			{
				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
			}

			ILubyte* imageData = ilGetData();
			int width = ilGetInteger(IL_IMAGE_WIDTH);
			int height = ilGetInteger(IL_IMAGE_HEIGHT);

			glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_FORMAT), width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, imageData);

			if (useMipMaps)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			}
			ilDeleteImages(1, &imageId);

			glBindTexture(GL_TEXTURE_2D, 0);

			textures.insert(std::make_pair(path, TextureObject(path, static_cast<uint>(textureId), width, height)));

			delete[] data;
			return textures[path];
		}
		delete[] data;
	}
	return textures["BLACK_FALLBACK"];
}

const TextureObject& ModuleTextures::Get(const std::string& path)
{
	const auto textureId = textures.find(path);
	if (textureId != textures.end())
		return (*textureId).second;

	TTLOG("### Error getting texture. Not found ###\n");
	return textures["BLACK_FALLBACK"];
}

bool ModuleTextures::Find(const std::string& path) const
{
	auto textureId = textures.find(path);
	if (textureId != textures.end())
		return true;

	return false;
}