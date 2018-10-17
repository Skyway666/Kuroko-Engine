#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include "Module.h"
class FileSystem: public Module {
public:
	FileSystem(Application* app, bool start_enabled = true);
	~FileSystem();

	void ExportBuffer(char* data,int size, const char* file_name); // get buffer from a file
	char* ImportFile(char* file_name); // recieve buffer from a file
};

#endif

