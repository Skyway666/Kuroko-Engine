#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include "Module.h"
#include "Resource.h"


#define ENGINE_EXTENSION ".kr"
#define JSON_EXTENSION ".json"
#define DDS_EXTENSION ".dds"
#define META_EXTENSION ".meta"

#define MESHES_FOLDER "Library\\Meshes\\"
#define MATERIALS_FOLDER "Library\\Materials\\"
#define ANIMATIONS_FOLDER "Library\\Animations\\"
#define TEXTURES_FOLDER "Library\\Textures\\"
#define PREFABS_FOLDER "Library\\Prefabs\\"
#define SETTINGS_FOLDER "Settings\\"
#define SCENES_FOLDER "Assets\\Scenes\\"
#define ASSETS_FOLDER "Assets"

enum lib_dir {
	LIBRARY_MESHES,
	LIBRARY_MATERIALS, // Unused (should be a json pointing to different textures)
	LIBRARY_ANIMATIONS,// Unused
	LIBRARY_TEXTURES,
	LIBRARY_PREFABS,
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
	bool getPath(std::string & str);
	void getExtension(std::string& str);
	void FormFullPath(std::string& path, const char* file_name, lib_dir lib, const char* extension);
	bool copyFileTo(const char* full_path_src, lib_dir dest_lib = NO_LIB, const char* extension = "", std::string file_new_name = "");
	int getFileLastTimeMod(const char* file_name, lib_dir lib = NO_LIB, const char* extension = "");
	bool FindInDirectory(const char* directory, const char* file_name, std::string final_path); // File name with extension
	std::string getPathFromLibDir(lib_dir r_type);
	private: 
	// Useful functions to work with paths
	bool removeExtension(std::string& str);
	bool removePath(std::string& str);

};

#endif

