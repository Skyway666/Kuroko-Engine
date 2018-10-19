#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include "Module.h"
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
	void ExportBuffer(char* data,int size, const char* file_name, lib_dir lib = NO_LIB); // get buffer from a file
	char* ImportFile(const char* file_name); // recieve buffer from a file
};

#endif

