#include "FileSystem.h"
#include <iostream>
#include <fstream>


FileSystem::FileSystem(Application* app, bool start_enabled):Module(app, start_enabled){
	name = "FileSystem";
}


FileSystem::~FileSystem() {
}

void FileSystem::ExportBuffer(char * data, int size, const char * file_name) {
	// TODO: Be able to export the file to a certain folder (Library->meshes, Library->Materials...)
	std::ofstream file;
	std::string path = "Library\\Meshes\\";
	path.append(file_name);
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
