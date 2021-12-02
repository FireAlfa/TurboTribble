#ifndef __MODULE_FILE_SYSTEM_H__
#define __MODULE_FILE_SYSTEM_H__

#include "Module.h"
#include <vector>
#include <string>



struct SDL_RWops;
int closeSDLRWops(SDL_RWops* rw);

struct aiFileIO;

//struct BASS_FILEPROCS;
class Config;
struct PathNode;

class ModuleFileSystem : public Module
{
public:

	// Constructor
	ModuleFileSystem(Application* app, bool startEnabled = true);//const char* game_path = nullptr);
	// Destructor
	~ModuleFileSystem();

	// Called before render is available
	bool Init() override;

	// Called before quitting
	bool CleanUp() override;


	// Creates Library Directories
	void CreateLibraryDirectories();


	// ----- Utility functions -----
	
	bool AddPath(const char* pathOrZip);
	bool Exists(const char* file) const;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) const;
	const char* GetWriteDir() const;
	void DiscoverFiles(const char* directory, std::vector<std::string>& fileList, std::vector<std::string>& dirList) const;
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& fileList) const;
	void GetRealDir(std::string path, std::string& output) const;
	std::string GetPathRelativeToAssets(const char* originalPath) const;
	
	bool Read(const std::string& path, void* data, unsigned size) const; // reads from path and allocates in data. NOTE: The caller should be responsible to clean it
	bool Exists(const std::string& path) const;
	unsigned Size(const std::string& path) const;

	bool HasExtension(const char* path) const;
	bool HasExtension(const char* path, std::string extension) const;
	bool HasExtension(const char* path, std::vector<std::string> extensions) const;

	std::string NormalizePath(const char* path) const;
	void SplitFilePath(const char* fullPath, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;
	// -----------------------------


	// ----- Open for Read/Write -----
	
	unsigned int Load(const char* path, const char* file, char** buffer) const;
	unsigned int Load(const char* file, char** buffer) const;
	// -------------------------------


	// ----- Saving -----
	
	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);
	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) const;
	// ------------------


	// ----- Naming -----
	
	std::string GetUniqueName(const char* path, const char* name) const;
	std::string SetNormalName(const char* path);
	// ------------------


	std::string systemBasePath;
};

#endif // __MODULEFILESYSTEM_H__