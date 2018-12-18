#include "ResourceScene.h"
#include "Application.h"
#include "ModuleScene.h"


ResourceScene::ResourceScene(resource_deff deff): Resource(deff) {
}


ResourceScene::~ResourceScene() {
}

void ResourceScene::LoadToMemory() {
	App->scene->AskSceneLoadFile((char*)binary.c_str());
}

void ResourceScene::UnloadFromMemory() {
}
