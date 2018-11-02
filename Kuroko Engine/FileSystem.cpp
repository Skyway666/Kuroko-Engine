#include "FileSystem.h"
#include "Applog.h"
#include <iostream>
#include <fstream>



FileSystem::FileSystem(Application* app, bool start_enabled):Module(app, start_enabled){
	name = "FileSystem";
}


FileSystem::~FileSystem() {
}

void FileSystem::ExportBuffer(char * data, int size, const char * file_name, lib_dir lib, const char* extension) {

	std::string path = "";
	FormFullPath(path, file_name, lib, extension);
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


void FileSystem::CreateEmptyFile(const char* file_name, lib_dir lib, const char* extension) {

	std::string path = "";
	FormFullPath(path, file_name, lib, extension);
	std::ofstream file;
	file.open(path);
	file.close();
}

void FileSystem::DestroyFile(const char * file_name, lib_dir lib, const char * extension) {
	std::string path = "";
	FormFullPath(path, file_name, lib, extension);
	remove(path.c_str());
}

bool FileSystem::ExistisFile(const char * file_name, lib_dir lib, const char * extension) {
	std::string path = "";
	FormFullPath(path, file_name, lib, extension);

	std::ifstream file(path);
	bool ret = file.good();
	file.close();
	return ret;
}

void FileSystem::FormFullPath(std::string & path, const char * file_name, lib_dir lib, const char * extension) {

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
	case LIBRARY_TEXTURES:
		path = TEXTURES_FOLDER;
		break;
	case SETTINGS:
		path = SETTINGS_FOLDER;
		break;
	case ASSETS_SCENES:
		path = SCENES_FOLDER;
		break;
	case NO_LIB:
		path = "";
		break;
	}
	path.append(file_name);
	path.append(extension);
}

bool FileSystem::copyFileTo(const char * full_path_src, lib_dir dest_lib, const char * extension) {

	// Form destination path
	std::string dest_path;
	std::string file_name = full_path_src;
	getFileNameFromPath(file_name);
	FormFullPath(dest_path, file_name.c_str(), dest_lib, extension); 

	if (dest_path.compare(full_path_src) == 0) {
		app_log->AddLog("Trying to copy/paste same file in same location, avoiding operation");
		return false;
	}
	// Copy and paste
	std::ifstream src(full_path_src, std::ios::binary);
	std::ofstream dest(dest_path, std::ios::binary);
	dest << src.rdbuf();

	return src && dest;
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