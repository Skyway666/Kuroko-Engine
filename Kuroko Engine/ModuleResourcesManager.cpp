#include "ModuleResourcesManager.h"
#include "FileSystem.h"
#include "Application.h"
#include "Random.h"
#include "ModuleImporter.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "Applog.h"

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

	return UPDATE_CONTINUE;
}

update_status ModuleResourcesManager::PostUpdate(float dt)
{
	// Debug purpuses
	if (reloadResources) {
		CleanMeta();
		CleanLibrary();
		CleanUp();
		GenerateLibraryAndMeta();
		reloadResources = false;
	}
	if (cleanResources) {
		CleanMeta();
		CleanLibrary();
		CleanUp();
		cleanResources = false;
	}

	return UPDATE_CONTINUE;
}




Resource * ModuleResourcesManager::newResource(resource_deff deff) {
	Resource* ret = nullptr;

	switch (deff.type) {
	case R_TEXTURE: ret = (Resource*) new ResourceTexture(deff); break;
	case R_MESH: ret = (Resource*) new ResourceMesh(deff); break;     
	case R_SCENE: ret = (Resource*) new ResourceScene(deff); break; 
	} 

	if (ret)
		resources[deff.uuid] = ret;

	return ret;
}


void ModuleResourcesManager::GenerateLibraryAndMeta()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		resource_deff new_res_deff;
		if (ManageFile(it.path().generic_string(), new_res_deff))
			newResource(new_res_deff);
	}
}

bool ModuleResourcesManager::ManageFile(std::string file_path, resource_deff& deff)
{
	std::string path, name, extension;
	path = name = extension = file_path;	// Separate path, name and extension	
	App->fs.getExtension(extension);
	App->fs.getPath(path);
	App->fs.getFileNameFromPath(name);

	if (extension == JSON_EXTENSION) // Scenes of our own engine are NOT to be exported to binary, they directly hold pointers to it.
		return false;
	// Manage meta
	if (extension == META_EXTENSION) { // If it is a meta file
		ManageMeta(path, name, extension);  // If has a corresponding asset, continue, else delete file from library and delete .meta
		return false;
	}
	// Manage asset
	deff = ManageAsset(path, name, extension);
	return true;
}

void ModuleResourcesManager::ManageMeta(std::string path, std::string name, std::string extension) {

	std::string full_meta_path = path + name + extension;

	JSON_Value* meta = json_parse_file(full_meta_path.c_str());
	std::string full_asset_path = path + name;

	if (App->fs.ExistisFile(full_asset_path.c_str())) // If file exists, MISSION ACOMPLISHED, return
	{
		json_value_free(meta);
		return;
	}
	else {	
		std::string full_binary_path = json_object_get_string(json_object(meta), "binary_path");		// Delete file from library using the meta reference that points to it
		ResourceType r_type = type2enumType(json_object_get_string(json_object(meta), "type"));

		if (r_type == R_SCENE) {									// Handle meshes delete when a scene is deleted
			CleanMeshesFromLibrary(full_binary_path.c_str());
		}
		App->fs.DestroyFile(full_binary_path.c_str());	// Destroy binary			
		App->fs.DestroyFile(full_meta_path.c_str()); // Destroy meta
		json_value_free(meta);
	}


}

void ModuleResourcesManager::CleanMeshesFromLibrary(std::string prefab_binary)
{
	JSON_Value* scene = json_parse_file(prefab_binary.c_str());
	JSON_Array* objects = json_object_get_array(json_object(scene), "Game Objects");

	// Delete meshes from library
	for (int i = 0; i < json_array_get_count(objects); i++) {
		JSON_Object* obj_deff = json_array_get_object(objects, i);
		JSON_Array* components = json_object_get_array(obj_deff, "Components");
		std::string mesh_binary;
		// Iterate components and look for the mesh
		bool mesh_found = false;
		for (int a = 0; a < json_array_get_count(components); a++) {
			JSON_Object* mesh_resource = json_array_get_object(components, a);
			std::string type = json_object_get_string(mesh_resource, "type");
			if (type == "mesh") {
				mesh_binary = json_object_get_string(mesh_resource, "mesh_binary_path");
				mesh_found = true;
				break;
			}
		}

		if (mesh_found) {					  // If a mesh was found destroy its binary
			App->fs.DestroyFile(mesh_binary.c_str());
		}
	}

	json_value_free(scene);
	json_array_clear(objects);
	
}

resource_deff ModuleResourcesManager::ManageAsset(std::string path, std::string name, std::string extension) {

	JSON_Value* meta;
	int file_last_mod = 0;
	// Needed for collisioning .meta against asset
	std::string full_meta_path = path + name + extension + META_EXTENSION; // TODO: Add original extension to .meta
	std::string full_asset_path = path + name + extension;
	// Needed for import
	ResourceType enum_type;
	std::string uuid_str;
	std::string binary_path;
	uint uuid_number = 0;
	resource_deff deff;
	if (App->fs.ExistisFile(full_meta_path.c_str())) {		// Check if .meta file exists
		meta = json_parse_file(full_meta_path.c_str());
		file_last_mod = App->fs.getFileLastTimeMod(full_asset_path.c_str());
		if (json_object_get_number(json_object(meta), "timeCreated") == file_last_mod) { // Check if the last time that was edited is the .meta timestamp
			deff.type = type2enumType(json_object_get_string(json_object(meta), "type"));
			deff.binary = json_object_get_string(json_object(meta), "binary_path");
			deff.asset = full_asset_path;
			deff.uuid = json_object_get_number(json_object(meta), "resource_uuid");
			json_value_free(meta);
			return deff;																		// Existing meta, and timestamp is the same, generate a resource to store it in the code
		}
	}
	else {
		App->fs.CreateEmptyFile(full_meta_path.c_str());	// If .meta doesn't exist, generate it
		meta = json_value_init_object();
		std::string str_type;
		uuid_number = random32bits();
		uuid_str = uuid2string(uuid_number);
		str_type = assetExtension2type(extension.c_str());
		enum_type = type2enumType(str_type.c_str());
		
		binary_path = App->fs.getPathFromLibDir(enumType2libDir(enum_type)) + uuid_str + enumType2binaryExtension(enum_type);
		file_last_mod = App->fs.getFileLastTimeMod(full_asset_path.c_str());
		json_object_set_number(json_object(meta), "resource_uuid", uuid_number);
		json_object_set_string(json_object(meta), "string_uuid", uuid_str.c_str());			// Brand new uuid
		json_object_set_string(json_object(meta), "type", str_type.c_str()); // Brand new time
		json_object_set_string(json_object(meta), "binary_path", binary_path.c_str()); // Brand new binary path
	}

	// If meta didn't exist, or existed but the asset was changed, 
	json_object_set_number(json_object(meta), "timeCreated", file_last_mod);	// Brand new / Updated time modification
	json_serialize_to_file(meta, full_meta_path.c_str());
	json_value_free(meta);

	// TODO: Import file
	switch (enum_type) {
		case R_TEXTURE:
			App->importer->ImportTexture(full_asset_path.c_str(), uuid_str);
			break;
		case R_SCENE:
			App->importer->ImportScene(full_asset_path.c_str(), uuid_str);
			break;
	}
	// Meta generated and file imported, create resource in code

	deff.type = enum_type;
	deff.asset = full_asset_path;
	deff.binary = binary_path;
	deff.uuid = uuid_number;

	// Resource generated, MISSION ACOMPLISHED, return
	return deff;
}


void ModuleResourcesManager::LoadResource(uint uuid) {
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->LoadToMemory();
	}
	else
		app_log->AddLog("WARNING: Trying to load non existing resource");
}

Resource * ModuleResourcesManager::getResource(uint uuid) {

	Resource* ret = nullptr;
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		ret = resources[uuid];
	}
	else
		app_log->AddLog("WARNING: Asking for non existing resource");

	return ret;
}

int ModuleResourcesManager::assignResource(uint uuid) {

	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->components_used_by++;
		if (!resources[uuid]->IsLoaded()) {
			resources[uuid]->LoadToMemory();
		}
	}
	else
		app_log->AddLog("WARNING: Trying to assing non existing resource");

	return 0;
}

int ModuleResourcesManager::deasignResource(uint uuid) {
	auto it = resources.find(uuid);
	if (it != resources.end()) {
		resources[uuid]->components_used_by--;
		if (resources[uuid]->components_used_by == 0) {
			resources[uuid]->UnloadFromMemory();
		}
	}
	else
		app_log->AddLog("WARNING: Trying to deasign non existing resource");

	return 0;
}

void ModuleResourcesManager::LoadFileToScene(const char * file) {
	std::string full_meta_path = file;
	full_meta_path += META_EXTENSION;
	uint resource_uuid = -1;

	if (App->fs.ExistisFile(full_meta_path.c_str())) {
		JSON_Value* meta = json_parse_file(full_meta_path.c_str());
		resource_uuid = json_object_get_number(json_object(meta), "resource_uuid");
		json_value_free(meta);

		resources[resource_uuid]->LoadToMemory();
	}
	else{
		app_log->AddLog("%s has no .meta, can't be loaded", file);
		return;
	}


	
}

void ModuleResourcesManager::CleanMeta() {
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(ASSETS_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;

		std::string path, name, extension;
		path = name = extension = it.path().generic_string();	// Separate path, name and extension	
		App->fs.getExtension(extension);
		App->fs.getPath(path);
		App->fs.getFileNameFromPath(name);

		if (extension == META_EXTENSION)
			App->fs.DestroyFile((path + name + extension).c_str());

	}
}

void ModuleResourcesManager::CleanLibrary()
{
	using std::experimental::filesystem::recursive_directory_iterator;
	for (auto& it : recursive_directory_iterator(LIBRARY_FOLDER)) {
		if (it.status().type() == std::experimental::filesystem::v1::file_type::directory) // If the path is a directory, ignore it
			continue;
		App->fs.DestroyFile(it.path().generic_string().c_str());
	}
}

std::string ModuleResourcesManager::uuid2string(uint uuid) {
	return std::to_string(uuid);
}

const char * ModuleResourcesManager::assetExtension2type(const char * extension) {

	std::string str_ex = extension;
	char* ret = "unknown";

	if (str_ex == ".FBX" || str_ex == ".fbx" || str_ex == ".dae" || str_ex == ".blend" || str_ex == ".3ds" || str_ex == ".obj"
		|| str_ex == ".gltf" || str_ex == ".glb" || str_ex == ".dxf" || str_ex == ".x") {
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
			ret = ".dds";
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

lib_dir ModuleResourcesManager::enumType2libDir(ResourceType type) {
	lib_dir ret = NO_LIB;
	switch (type) {
	case R_TEXTURE:
		ret = LIBRARY_TEXTURES;
		break;
	case R_MESH:
		ret = LIBRARY_MESHES;
		break;
	case R_SCENE:
		ret = LIBRARY_PREFABS;
		break;
	}
	return ret;
}


bool ModuleResourcesManager::CleanUp() {
	// Unload all memory from resources and delete resources
	for (auto it = resources.begin(); it != resources.end(); it++) {
		if (Resource* resource = (*it).second) {
			if (resource->IsLoaded())
				resource->UnloadFromMemory();
			delete resource;
		}
	}
	resources.clear();

	return true;
}