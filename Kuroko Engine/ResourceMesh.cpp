#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "Applog.h"



ResourceMesh::ResourceMesh(resource_deff deff): Resource(deff) {
	
}


ResourceMesh::~ResourceMesh() {
}

void ResourceMesh::LoadToMemory() {
	mesh = App->importer->ImportMeshFromKR(binary.c_str());

	if (!mesh)
		app_log->AddLog("Error loading binary %s", binary.c_str());
	else
		loaded_in_memory = true;
}

void ResourceMesh::UnloadFromMemory() {
	if (!mesh)
		return;
	delete mesh;
	mesh = nullptr;
	loaded_in_memory = false;
}
