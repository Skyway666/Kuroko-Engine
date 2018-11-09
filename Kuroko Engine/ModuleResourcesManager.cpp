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

		std::string path, name, extension;	// Separate path and name, to reach the .meta with the same name		

		extension = path = name = it.path().generic_string();
		App->fs->getExtension(extension);
		App->fs->getPath(path);
		App->fs->getFileNameFromPath(name);

		// Manage meta
		if (extension == META_EXTENSION) { // If it is a meta file
			ManageMeta(path, name, extension);  // If has a corresponding asset, continue, else delete file from library and delete .meta
			continue;
		}
		// Manage asset
		ManageAsset(path, name, extension);
	}

}

void ModuleResourcesManager::ManageMeta(std::string path, std::string name, std::string extension) {
	std::string full_meta_path = path + name + extension;
	JSON_Value* meta = json_parse_file(full_meta_path.c_str());
	std::string asset_extension = json_object_get_string(json_object(meta), "asset_extension");
	std::string full_asset_path = path + name + asset_extension;

	if (App->fs->ExistisFile(full_asset_path.c_str())) // If file exists, MISSION ACOMPLISHED, return
		return;
	else {											   // Delete the corresponding file from the library and delete the meta
		//TODO: Delete file from library using the meta reference that points to it

		App->fs->DestroyFile(full_meta_path.c_str());
	}


}

void ModuleResourcesManager::ManageAsset(std::string path, std::string name, std::string extension) {

	JSON_Value* meta;
	int file_last_mod;
	std::string full_meta_path = path + name + META_EXTENSION;
	std::string full_asset_path = path + name + extension;
	if (App->fs->ExistisFile(full_meta_path.c_str())) {		// Check if .meta file exists
		meta = json_parse_file(full_meta_path.c_str());
		file_last_mod = App->fs->getFileLastTimeMod(full_asset_path.c_str());
		if (json_object_get_number(json_object(meta), "timeCreated") == file_last_mod) // Check if the last time that was edited is the .meta timestamp, if it is, continue, nothing else to do
			return;																	   // Existing meta, and timestamp is the same, MISSION ACOMPLISHED, return
	}
	else {
		App->fs->CreateEmptyFile(full_meta_path.c_str());	// If .meta doesn't exist, generate it
		meta = json_value_init_object();
		file_last_mod = App->fs->getFileLastTimeMod(full_asset_path.c_str());
		json_object_set_number(json_object(meta), "uuid", random32bits());			// Brand new uuid
		json_object_set_string(json_object(meta), "asset_extension", extension.c_str()); // Brand new extension
		json_object_set_string(json_object(meta), "type", extension2type(extension.c_str())); // Brand new time
	}

	// If meta didn't exist, or existed but the asset was changed, 
	json_object_set_number(json_object(meta), "timeCreated", file_last_mod);	// Brand new / Updated time modification
	json_serialize_to_file(meta, full_meta_path.c_str());
	json_value_free(meta);

	// IMPORT FILE

	// Meta generated and file exported, MISSION ACOMPLISHED, return
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

const char * ModuleResourcesManager::extension2type(const char * extension) {
	char* ret = "unknown";

	if (extension == ".FBX" || extension == ".fbx" || extension == ".dae" || extension == ".blend" || extension == ".3ds" || extension == ".obj"
		|| extension == ".gltf" || extension == ".glb" || extension == ".dxf" || extension == ".x") {
		ret = "mesh";
	}
	if (extension == ".bmp" || extension == ".dds" || extension == ".jpg" || extension == ".pcx" || extension == ".png"
		|| extension == ".raw" || extension == ".tga" || extension == ".tiff") {
		ret = "texture";
	}
	if (extension == ".json") {
		ret = "scene";
	}

	return ret;
}


