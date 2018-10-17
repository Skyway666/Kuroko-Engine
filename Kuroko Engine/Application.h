#ifndef __APPLICATION
#define __APPLICATION

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include <list>
#include <vector>

class ModuleWindow;
class ModuleInput;
//class ModuleAudio;
class ModuleScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleUI;
class ModuleImporter;
class ModuleDebug;
class FileSystem;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	//ModuleAudio* audio;
	ModuleScene* scene;
	ModuleDebug* debug;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleUI* gui;
	ModuleImporter* importer;
	FileSystem* fs;

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
	void SaveConfig();
	void DeleteConfig();
	void LoadDefaultConfig();


	void requestBrowser(std::string link);
	std::vector<float> fps_log;
	std::vector<float> ms_log;
	int vector_limit; 
	std::string config_file_name, custom_config_file_name;


private:

	void SaveConfig_Real();
	void DeleteConfig_Real();
	void LoadDefaultConfig_Real();
	void PrepareUpdate();
	void FinishUpdate();
	bool want_to_save_config = false;
	bool want_to_delete_config = false;
	bool want_to_load_default_config = false;
};

extern Application* App;

#endif