#include "ModuleResourcesManager.h"



ModuleResourcesManager::ModuleResourcesManager(Application* app, bool start_enabled): Module(app, start_enabled)
{
	name = "resource manager";
}


ModuleResourcesManager::~ModuleResourcesManager()
{

}

bool ModuleResourcesManager::Init(const JSON_Object * config)
{
	return true;
}

bool ModuleResourcesManager::Start()
{
	return false;
}

update_status ModuleResourcesManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResourcesManager::CleanUp()
{
	return true;
}

Resource * ModuleResourcesManager::ExportToLibrary(const char * file)
{
	return nullptr;
}

void ModuleResourcesManager::GenerateLibraryFromAssets()
{
}

void Resource::LoadToMemory()
{
}

void Resource::UnloadFromMemory()
{
}

bool Resource::IsLoaded()
{
	return false;
}
