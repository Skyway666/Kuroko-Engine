#include "ResourceTexture.h"
#include "Material.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "Applog.h"


ResourceTexture::ResourceTexture(resource_deff deff): Resource(deff) {

}


ResourceTexture::~ResourceTexture() {

}

void ResourceTexture::LoadToMemory() {
	texture = App->importer->LoadTextureFromLibrary(binary.c_str());

	if(!texture)
		app_log->AddLog("Error loading binary %s", binary.c_str());

}

void ResourceTexture::UnloadFromMemory() {
	delete texture;
	texture = nullptr;
}
