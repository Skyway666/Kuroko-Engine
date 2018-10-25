#include "FileSystem.h"
#include <iostream>
#include <fstream>


FileSystem::FileSystem(Application* app, bool start_enabled):Module(app, start_enabled){
	name = "FileSystem";
}


FileSystem::~FileSystem() {
}

void FileSystem::ExportBuffer(char * data, int size, const char * file_name, lib_dir lib, const char* extension) {

	// use CreateDirectory() to create folders at the beggining of the app
	std::string path = "";
	switch (lib) {
		case LIBRARY_MESHES:
			path = MESHES_FOLDER;
			break;
		case LIBRARY_MATERIALS:
			path = MATERIALS_FOLDER;
			break;
		case LIBRARY_ANIMATIONS:
			path = ANIMATIONS_FOLDER;
			break;
		case NO_LIB:
			path = "";
			break;
	}
	path.append(file_name);
	path.append(extension);
	std::ofstream file;
	file.open(path, std::fstream::out | std::fstream::binary);
	file.write(data, size);
	file.close();
}

char * FileSystem::ImportFile(const char * file_name) {
	std::ifstream file;
	file.open(file_name, std::fstream::out | std::fstream::binary);
	// get length of file:
	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	char* ret = new char[size];
	file.read(ret, size);

	return ret;
}

bool FileSystem::removeExtension(std::string& str) {
	size_t lastdot = str.find_last_of(".");
	if (lastdot == std::string::npos)
		return false;
	str = str.substr(0, lastdot);
	return true;
}

bool FileSystem::removePath(std::string& str) {
	const size_t last_slash_idx = str.find_last_of("\\/");
	if (last_slash_idx == std::string::npos)
		return false;
	str = str.erase(0, last_slash_idx + 1);
	return true;
}

void FileSystem::getFileNameFromPath(std::string & str) {
	removeExtension(str);
	removePath(str);
}