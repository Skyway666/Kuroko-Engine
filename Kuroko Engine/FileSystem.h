#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include "Module.h"


#define ENGINE_EXTENSION ".kr"
#define JSON_EXTENSION ".json"
#define MESHES_FOLDER "Library\\Meshes\\"
#define MATERIALS_FOLDER "Library\\Materials\\"
#define ANIMATIONS_FOLDER "Library\\Animations\\"
#define SETTINGS_FOLDER "Settings\\"
#define SCENES_FOLDER "Assets\\Scenes\\"
enum lib_dir {
	LIBRARY_MESHES,
	LIBRARY_MATERIALS,
	LIBRARY_ANIMATIONS,
	SETTINGS,
	ASSETS_SCENES,
	NO_LIB
};
class FileSystem: public Module {
public:
	FileSystem(Application* app, bool start_enabled = true);
	~FileSystem();

	public:
	void ExportBuffer(char* data,int size, const char* file_name, lib_dir lib = NO_LIB, const char* extension = ""); // get buffer from a file
	char* ImportFile(const char* file_name); // recieve buffer from a file

	void CreateEmptyFile(const char* file_name, lib_dir lib = NO_LIB, const char* extension = "");
	void DestroyFile(const char* file_name, lib_dir lib = NO_LIB, const char* extension = "");
	bool ExistisFile(const char* file_name, lib_dir lib = NO_LIB, const char* extension = "");
	void getFileNameFromPath(std::string& str);
	void FormFullPath(std::string& path, const char* file_name, lib_dir lib, const char* extension);
	private: 
	// Useful functions to work with paths
	bool removeExtension(std::string& str);
	bool removePath(std::string& str);

};

#endif

