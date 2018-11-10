#include "ModuleResourcesManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "Random.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"

// Temporal debug purposes
#include "ModuleInput.h"
#include <experimental/filesystem>
#include <iostream>

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
	GenerateLibraryAndMeta();
	return true;
}

update_status ModuleResourcesManager::Update(float dt)
{
	// Debug purpuses
	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN) {
		CleanMeta();
	}
	return UPDATE_CONTINUE;
}

bool ModuleResourcesManager::CleanUp()
{
	return true;
}

Resource * ModuleResourcesManager::getResource(uint uuid) {
	return nullptr;
}

Resource * ModuleResourcesManager::newResource(resource_deff deff) {
	Resource* ret = nullptr;

	switch (deff.type) {
	case R_TEXTURE: ret = (Resource*) new ResourceTexture(deff); break;
	//case R_MESH: ret = (Resource*) new ResourceMesh(deff); break;
	//case R_SCENE: ret = (Resource*) new ResourceScene(deff); break;
	} 

	if (ret)
		resources[deff.uuid] = ret;

	return ret;
}

void ModuleResourcesManager::ImportToLibrary(const char* file_original_name, std::string file_binary_name)
{

}

void ModuleResourcesManager::GenerateLibraryAndMeta()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		std::string path, name, extension;
		path = name = extension = it.path().generic_string();	// Separate path, name and extension	
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
	std::string full_asset_path = path + name + asset_extension; // Generate path to asset using "asset_extension" in the metadata

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
	// Needed for collisioning .meta against asset
	std::string full_meta_path = path + name + META_EXTENSION; // TODO: Add original extension to .meta
	std::string full_asset_path = path + name + extension;
	// Needed for import
	ResourceType asset_type;
	std::string uuid;
	if (App->fs->ExistisFile(full_meta_path.c_str())) {		// Check if .meta file exists
		meta = json_parse_file(full_meta_path.c_str());
		file_last_mod = App->fs->getFileLastTimeMod(full_asset_path.c_str());
		if (json_object_get_number(json_object(meta), "timeCreated") == file_last_mod) { // Check if the last time that was edited is the .meta timestamp
			resource_deff deff;
			std::string binary_name = json_object_get_string(json_object(meta), "uuid");
			deff.type = type2enumType(json_object_get_string(json_object(meta), "type"));
			deff.asset = full_asset_path;
			deff.binary = binary_name + enumType2binaryExtension(deff.type);
			deff.uuid = atoi(binary_name.c_str());
			newResource(deff);
			return;																		// Existing meta, and timestamp is the same, generate a resource to store it in the code
		}
	}
	else {
		App->fs->CreateEmptyFile(full_meta_path.c_str());	// If .meta doesn't exist, generate it
		meta = json_value_init_object();
		std::string type;
		uuid = uuid2string(random32bits());
		type = extension2type(extension.c_str());
		asset_type = type2enumType(type.c_str());
		file_last_mod = App->fs->getFileLastTimeMod(full_asset_path.c_str());
		json_object_set_string(json_object(meta), "uuid", uuid.c_str());			// Brand new uuid
		json_object_set_string(json_object(meta), "asset_extension", extension.c_str()); // Brand new extension (TODO: Delete this when file has original extension in it
		json_object_set_string(json_object(meta), "type", type.c_str()); // Brand new time
	}

	// If meta didn't exist, or existed but the asset was changed, 
	json_object_set_number(json_object(meta), "timeCreated", file_last_mod);	// Brand new / Updated time modification
	json_serialize_to_file(meta, full_meta_path.c_str());
	json_value_free(meta);

	// TODO: Import file
	switch (asset_type) {
		case R_TEXTURE:
			App->importer->ImportTexture(full_asset_path.c_str(), uuid);
			break;
		case R_SCENE:
			App->importer->ImportScene(full_asset_path.c_str(), uuid);
			break;
	}


	// Meta generated and file exported, MISSION ACOMPLISHED, return
}


void ModuleResourcesManager::LoadResource(uint uuid) {
}

void ModuleResourcesManager::LoadFileToScene(const char * file) {
}

void ModuleResourcesManager::CleanMeta() {
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		std::string path, name, extension;
		path = name = extension = it.path().generic_string();	// Separate path, name and extension	
		App->fs->getExtension(extension);
		App->fs->getPath(path);
		App->fs->getFileNameFromPath(name);

		if (extension == META_EXTENSION)
			App->fs->DestroyFile((path + name + extension).c_str());

	}
}

std::string ModuleResourcesManager::uuid2string(uint uuid) {
	return std::to_string(uuid);
}

const char * ModuleResourcesManager::extension2type(const char * extension) {

	std::string str_ex = extension;
	char* ret = "unknown";

	if (str_ex == ".FBX" || str_ex == ".fbx" || str_ex == ".dae" || str_ex == ".blend" || str_ex == ".3ds" || str_ex == ".obj"
		|| str_ex == ".gltf" || str_ex == ".glb" || str_ex == ".dxf" || str_ex == ".x" || str_ex == ".json") {
		ret = "scene";
	}
	if (str_ex == ".bmp" || str_ex == ".dds" || str_ex == ".jpg" || str_ex == ".pcx" || str_ex == ".png"
		|| str_ex == ".raw" || str_ex == ".tga" || str_ex == ".tiff") {
		ret = "texture";
	}

	return ret;
}

ResourceType ModuleResourcesManager::type2enumType(const char * type) {
	ResourceType ret = R_UNKNOWN;
	std::string str_type = type;

	if (str_type == "scene")
		ret = R_SCENE;
	if (str_type == "texture")
		ret = R_TEXTURE;

	return ret;
}

const char * ModuleResourcesManager::enumType2binaryExtension(ResourceType type) {
	const char* ret = "";
	switch (type) {
		case R_TEXTURE:
			ret = "dds";
			break;
		case R_MESH:
			ret = ".kr";
			break;
		case R_SCENE:
			ret = ".json";
			break;
	}

	return ret;
}



