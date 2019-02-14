#include "Resource3dObject.h"
#include "Parson\parson.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleResourcesManager.h"


Resource3dObject::Resource3dObject(resource_deff deff): Resource(deff){
	// Create resources for every mesh contained by the scene

	JSON_Value* scene = json_parse_file(binary.c_str());
	JSON_Array* objects = json_object_get_array(json_object(scene), "Game Objects");

	// Make resources for every mesh
	for (int i = 0; i < json_array_get_count(objects); i++) {
		JSON_Object* obj_deff = json_array_get_object(objects, i);
		JSON_Array* components = json_object_get_array(obj_deff, "Components");
		resource_deff deff;
		deff.type = R_MESH;

		// Iterate components and look for the mesh/animation
		for (int a = 0; a < json_array_get_count(components); a++) {
			bool mesh_found = false;
			bool animation_found = false;

			JSON_Object* resource = json_array_get_object(components, a);
			std::string type = json_object_get_string(resource, "type");
			if (type == "mesh") {
				deff.binary = json_object_get_string(resource, "mesh_binary_path");
				deff.uuid = json_object_get_number(resource, "mesh_resource_uuid");
				deff.asset = json_object_get_string(resource, "mesh_name");
				deff.Parent3dObject = asset;
				mesh_found = true;
			}
			else if (type == "animation")
			{
				deff.binary = json_object_get_string(resource, "animation_binary_path");
				deff.uuid = json_object_get_number(resource, "aimation_resource_uuid");
				deff.asset = json_object_get_string(resource, "animation_name");
				deff.Parent3dObject = asset;
				animation_found = true;
			}

			if (mesh_found || animation_found) {					  // If a mesh/animation was found create a resource for it
				App->resources->newResource(deff);				
			}
		}


	}

	json_value_free(scene);
}


Resource3dObject::~Resource3dObject() {

}

void Resource3dObject::LoadToMemory() {
	App->scene->AskPrefabLoadFile(binary.c_str(), float3(0, 0, 0), float3(0, 0, 0));
	loaded_in_memory = true;
}

void Resource3dObject::UnloadFromMemory() {
	loaded_in_memory = false;
}
