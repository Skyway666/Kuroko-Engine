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

Application::Application()
{
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleScene(this);
	renderer3D = new ModuleRenderer3D(this);
	renderer2D = new ModuleRenderer2D(this);
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
}

Application::~Application()
{
	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	APPLOG("Application Init --------------");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->Init();

	APPLOG("Application Start --------------");
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret; it++)
		ret = (*it)->Start();
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
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