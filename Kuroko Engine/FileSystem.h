#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include "Module.h"


#define ENGINE_EXTENSION ".kr"
#define MESHES_FOLDER "Library\\Meshes\\"
#define MATERIALS_FOLDER "Library\\Materials\\"
#define ANIMATIONS_FOLDER "Library\\Animations\\"
enum lib_dir {
	LIBRARY_MESHES,
	LIBRARY_MATERIALS,
	LIBRARY_ANIMATIONS,
	NO_LIB
};
class FileSystem: public Module {
public:
	FileSystem(Application* app, bool start_enabled = true);
	~FileSystem();

	// Pass the file only with extension and name
	void ExportBuffer(char* data,int size, const char* file_name, lib_dir lib = NO_LIB, const char* extension = ""); // get buffer from a file
	char* ImportFile(const char* file_name); // recieve buffer from a file
	// Useful functions to work with paths
	bool removeExtension(std::string& str);
	bool removePath(std::string& str);
	void getFileNameFromPath(std::string& str);
};

#endif

