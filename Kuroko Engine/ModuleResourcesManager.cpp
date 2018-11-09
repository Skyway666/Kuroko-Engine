#include "ModuleResourcesManager.h"
#include "FileSystem.h"
#include <experimental/filesystem>

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
	GenerateLibraryFromAssets();
	return true;
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
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		std::string path = it.path().generic_string();
		int a = 0;
	}

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


