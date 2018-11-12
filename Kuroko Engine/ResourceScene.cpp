#include "ResourceScene.h"
#include "Parson\parson.h"
#include "Application.h"
#include "ModuleResourcesManager.h"


ResourceScene::ResourceScene(resource_deff deff): Resource(deff){
	// Create resources for every mesh contained by the scene

	JSON_Value* scene = json_parse_file(binary.c_str());
	JSON_Array* objects = json_object_get_array(json_object(scene), "Game Objects");

	// Make resources for every mesh
	for (int i = 0; i < json_array_get_count(objects); i++) {
		JSON_Object* obj_deff = json_array_get_object(objects, i);
		JSON_Array* components = json_object_get_array(obj_deff, "Components");
		resource_deff deff;
		deff.asset = asset;
		deff.type = R_MESH;

		// Iterate components and look for the mesh
		bool mesh_found = false;
		for (int a = 0; a < json_array_get_count(components); a++) {
			JSON_Object* mesh_resource = json_array_get_object(components, a);
			std::string type = json_object_get_string(mesh_resource, "type");
			if (type == "mesh") {
				deff.binary = json_object_get_string(mesh_resource, "mesh_binary_path");
				deff.uuid = json_object_get_number(mesh_resource, "mesh_resource_uuid");
				mesh_found = true;
				break;
			}
		}

		if (mesh_found) {					  // If a mesh was found create a resource for it
			App->resources->newResource(deff);
		}
	}
}


ResourceScene::~ResourceScene() {

}

void ResourceScene::LoadToMemory() {
	loaded_in_memory = true;
}

void ResourceScene::UnloadFromMemory() {

	loaded_in_memory = false;
}
