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

Resource * ModuleResourcesManager::getResource(uint uuid) {
	return nullptr;
}

Resource * ModuleResourcesManager::newResource(uint uuid) {
	return nullptr;
}

uint ModuleResourcesManager::ImportToLibrary(const char * file)
{
	return 0;
}

void ModuleResourcesManager::GenerateLibraryFromAssets()
{
}

void ModuleResourcesManager::CreateResourcesFromMetadata() {
}

void ModuleResourcesManager::LoadResource(uint uuid) {
}

void ModuleResourcesManager::LoadFileToScene(const char * file) {
}

char * ModuleResourcesManager::uuid2string(uint uuid) {
	return nullptr;
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
