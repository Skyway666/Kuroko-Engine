#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleRenderer2D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleImGUI.h"
#include "ModuleImporter.h"
#include <iostream>
#include <fstream>

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	//renderer2D = new ModuleRenderer2D(this);
	importer = new ModuleImporter(this);
	camera = new ModuleCamera3D(this);
	physics = new ModulePhysics3D(this);
	gui = new ModuleImGUI(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	list_modules.push_back(window);
	list_modules.push_back(camera);
	list_modules.push_back(input);
	list_modules.push_back(audio);
	list_modules.push_back(importer);
	list_modules.push_back(physics);
	
	
	// Scenes
	list_modules.push_back(scene_intro);

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

	config_file_name = "JSON Files/config.json";
	custom_config_file_name = "JSON Files/custom_config.json";


	// Check if there is editor saved data, load custom if there is, load default if there isn't
	if (json_object_get_boolean(json_value_get_object(json_parse_file(custom_config_file_name.c_str())), "saved_data"))
		config = json_value_get_object(json_parse_file(custom_config_file_name.c_str()));
	else
		config = json_value_get_object(json_parse_file(config_file_name.c_str()));

	gui->getLog()->AddLog("Application Init --------------\n");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->Init(json_object_get_object(config, (*it)->name.c_str()));

	gui->getLog()->AddLog("Application Start --------------\n");
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

void Application::SaveConfig() {
	want_to_save_config = true;
}
void Application::DeleteConfig() {
	want_to_delete_config = true;
}
void Application::LoadDefaultConfig() {
	want_to_load_default_config = true;
}

void Application::SaveConfig_Real() {
	JSON_Value* config = json_parse_file(custom_config_file_name.c_str());

	json_object_set_boolean(json_object(config), "saved_data", true);

	// Pass one object to each module
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++){
		JSON_Value* module_config = json_value_init_object();
		(*it)->SaveConfig(json_object(module_config));
		json_object_set_value(json_object(config), (*it)->name.c_str(), module_config);
	}
	// Fill file with info
	json_serialize_to_file(config, custom_config_file_name.c_str());
}

void Application::DeleteConfig_Real() {
	JSON_Value* config = json_parse_file(custom_config_file_name.c_str());

	json_object_clear(json_object(config));
	json_object_set_boolean(json_object(config), "saved_data", false);

	json_serialize_to_file(config, custom_config_file_name.c_str());
}

void Application::LoadDefaultConfig_Real() {
	JSON_Object* config = json_value_get_object(json_parse_file(config_file_name.c_str()));

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); it++)
		(*it)->LoadConfig(json_object_get_object(config, (*it)->name.c_str()));
}