#pragma once
#include "Module.h"

class Scene;
class Mesh;

class ModuleFBXimporter : public Module
{
public:
	ModuleFBXimporter(Application* app, bool start_enabled = true);
	~ModuleFBXimporter();
	Mesh* LoadFBX(const char* file);

	bool Init();
	bool CleanUp();

};