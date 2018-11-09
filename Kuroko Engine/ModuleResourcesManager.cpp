#include "ModuleResourcesManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "Random.h"
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

		std::string path, name;									 // Separate path and name, to reach the .meta with the same name
		path = name = it.path().generic_string();
		App->fs->getPath(path);
		App->fs->getFileNameFromPath(name);

		std::string full_meta_path = path + name + META_EXTENSION;
		if (App->fs->ExistisFile(full_meta_path.c_str())) {		// Check if .meta file exists
			continue;											// TODO: Check if the last time that was edited is the .meta timestamp, if it is, continue
		}
			
		App->fs->CreateEmptyFile(full_meta_path.c_str());		// Generate .meta

		JSON_Value* meta = json_value_init_object();
		json_object_set_number(json_object(meta), "uuid", random32bits());

		json_serialize_to_file(meta, full_meta_path.c_str());
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


