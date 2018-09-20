#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include <list>

class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleScene;
class ModuleRenderer3D;
class ModuleRenderer2D;
class ModuleCamera3D;
class ModulePhysics3D;
class ModuleImGUI;
class ModuleImporter;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleScene* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleRenderer2D* renderer2D;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
	ModuleImGUI* gui;
	ModuleImporter* importer;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();
	void requestBrowser(std::string link);

private:

	void PrepareUpdate();
	void FinishUpdate();
};