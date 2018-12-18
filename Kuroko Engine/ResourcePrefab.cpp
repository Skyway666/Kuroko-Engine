#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleScene.h"


ResourcePrefab::ResourcePrefab(resource_deff deff): Resource(deff) {
}


ResourcePrefab::~ResourcePrefab() {
}

void ResourcePrefab::LoadToMemory() {
	App->scene->AskPrefabLoadFile(binary.c_str());
}

void ResourcePrefab::UnloadFromMemory() {
}
