#include "FileSystem.h"
#include <iostream>
#include <fstream>


FileSystem::FileSystem(Application* app, bool start_enabled):Module(app, start_enabled){
	name = "FileSystem";
}


FileSystem::~FileSystem() {
}

void FileSystem::ExportBuffer(char * data, int size, char * file_name) {
	std::ofstream file;
	file.open(file_name);
	file.write(data, size);
	file.close();
}

char * FileSystem::ImportFile(char * file_name) {
	std::ifstream file;
	file.open(file_name);
	// get length of file:
	file.seekg(0, file.end);
	int size = file.tellg();
	file.seekg(0, file.beg);
	char* ret = new char[size];
	file.readsome(ret, size);

	return ret;
}
