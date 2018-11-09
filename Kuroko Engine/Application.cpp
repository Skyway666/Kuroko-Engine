#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
//#include "ModuleAudio.h"
#include "ModuleScene.h"
#include "ModuleDebug.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleUI.h"
#include "ModuleImporter.h"
#include "AppLog.h"
#include "Random.h"
#include "FileSystem.h"
#include "ModuleTimeManager.h"
#include "ModuleResourcesManager.h"
#include <Windows.h>
#include <iostream>
#include <fstream>


Application::Application()
{
	randomizeSeed();

	// Create library directory if it does not exist
	CreateDirectory("Library", NULL);
	CreateDirectory("Library\\Meshes", NULL);
	CreateDirectory("Library\\Materials", NULL);
	CreateDirectory("Library\\Animation", NULL);
	CreateDirectory("Library\\Textures", NULL);
	CreateDirectory("Assets", NULL);
	CreateDirectory("Assets\\Scenes", NULL);

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	//audio = new ModuleAudio(this, true);   // dummy module until further implementation
	scene = new ModuleScene(this);
	debug = new ModuleDebug(this);
	renderer3D = new ModuleRenderer3D(this);
	//renderer2D = new ModuleRenderer2D(this);
	importer = new ModuleImporter(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleUI(this);
	fs = new FileSystem(this);
	time = new ModuleTimeManager(this);
	resources = new ModuleResourcesManager(this);


	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	list_modules.push_back(time);
	list_modules.push_back(resources);
	list_modules.push_back(window);
	list_modules.push_back(camera);
	list_modules.push_back(input);
	//list_modules.push_back(audio);
	list_modules.push_back(importer);

	
	
	// Scenes
	list_modules.push_back(scene);
	list_modules.push_back(debug);

	// Renderer last!
	//AddModule(renderer2D);
	list_modules.push_back(gui);
	list_modules.push_back(renderer3D);

	// HARDCODED
	vector_limit = 100;
}

Application::~Application()
{
	list_modules.clear();
	
}

bool Application::Init()
{
	bool ret = true;
	//JASON Stuff
	JSON_Object* config;


	config_file_name = "Settings/config.json";
	custom_config_file_name = "Settings/custom_config.json";

	bool custom_config = App->fs->ExistisFile("custom_config", SETTINGS, JSON_EXTENSION);

	App = this;

	// Check if there is editor saved data, load custom if there is, load default if there isn't
	if (custom_config)
		config = json_value_get_object(json_parse_file(custom_config_file_name.c_str()));
	else
		config = json_value_get_object(json_parse_file(config_file_name.c_str()));

	app_log->AddLog("Application Init --------------\n");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->Init(json_object_get_object(config, (*it)->name.c_str()));

	app_log->AddLog("Application Start --------------\n");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->Start();
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_log.push_back(ms_timer.Read());
	fps_log.push_back(1.0f / dt);
	if (ms_log.size() > vector_limit) { // ms and fps fill at the same time, so only need top check one
		ms_log.erase(ms_log.begin());
		fps_log.erase(fps_log.begin());
	}

	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (want_to_save_config) {
		SaveConfig_Real();
		want_to_save_config = false;
	}
	if (want_to_delete_config) {
		DeleteConfig_Real();
		want_to_delete_config = false;
	}
	if (want_to_load_default_config) {
		LoadDefaultConfig_Real();
		want_to_load_default_config = false;
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; it++)
		ret = (*it)->PreUpdate(dt);

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; it++)
		ret = (*it)->Update(dt);

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == UPDATE_CONTINUE; it++)
		ret = (*it)->PostUpdate(dt);

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->CleanUp();

	return ret;
}

void Application::requestBrowser(std::string link) {
	ShellExecute(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
}
void Application::SaveConfig_Real() {

	if (!App->fs->ExistisFile("custom_config", SETTINGS, JSON_EXTENSION))
		App->fs->CreateEmptyFile(custom_config_file_name.c_str());

	JSON_Value* config = json_value_init_object();
	// Pass one object to each module
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++){
		JSON_Value* module_config = json_value_init_object();
		(*it)->SaveConfig(json_object(module_config));
		json_object_set_value(json_object(config), (*it)->name.c_str(), module_config);
	}
	// Fill file with info
	json_serialize_to_file(config, custom_config_file_name.c_str());
	json_value_free(config);
}

void Application::DeleteConfig_Real() {
	App->fs->DestroyFile(custom_config_file_name.c_str());
}

void Application::LoadDefaultConfig_Real() {
	JSON_Value* config = json_parse_file(config_file_name.c_str());

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
		(*it)->LoadConfig(json_object_get_object(json_object(config), (*it)->name.c_str()));

	json_value_free(config);
}