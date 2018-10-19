#include "FileSystem.h"
#include <iostream>
#include <fstream>


FileSystem::FileSystem(Application* app, bool start_enabled):Module(app, start_enabled){
	name = "FileSystem";
}


FileSystem::~FileSystem() {
}

void FileSystem::ExportBuffer(char * data, int size, const char * file_name, lib_dir lib) {

	// use CreateDirectory() to create folders at the beggining of the app
	std::string path = "";
	switch (lib) {
		case LIBRARY_MESHES:
			path = "Library\\Meshes\\";
			break;
		case LIBRARY_MATERIALS:
			path = "Library\\Materials\\";
			break;
		case LIBRARY_ANIMATIONS:
			path = "Library\\Animations\\";
			break;
		case NO_LIB:
			path = "";
			break;
	}
	path.append(file_name);
	std::ofstream file;
	file.open(path);
	file.write(data, size);
	file.close();
}

char * FileSystem::ImportFile(const char * file_name) {
	std::ifstream file;
	file.open(file_name);
	// get length of file:
	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	char* ret = new char[size];
	file.read(ret, size);

	return ret;
}
