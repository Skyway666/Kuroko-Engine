#include "ResourceTexture.h"
#include "Material.h"
#include "Application.h"
#include "ModuleImporter.h"


ResourceTexture::ResourceTexture(resource_deff deff): Resource(deff) {

}


ResourceTexture::~ResourceTexture() {

}

void ResourceTexture::LoadToMemory() {
	texture = App->importer->LoadTextureFromLibrary(binary.c_str());
}

void ResourceTexture::UnloadFromMemory() {
}
