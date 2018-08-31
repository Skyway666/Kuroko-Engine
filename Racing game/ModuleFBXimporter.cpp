#include "ModuleFBXimporter.h"
#include "Globals.h"

#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "Mesh.h"

ModuleFBXimporter::ModuleFBXimporter(Application* app, bool start_enabled) : Module(app, start_enabled) {}

ModuleFBXimporter::~ModuleFBXimporter() {}


bool ModuleFBXimporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);



	return true;
}

bool ModuleFBXimporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

Mesh* ModuleFBXimporter::LoadFBX(const char* file)
{
	Mesh* current_mesh = nullptr;

	const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
	if (imported_scene != nullptr && imported_scene->HasMeshes())
	{

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i = 0; i < imported_scene->mNumMeshes; i++)
		{
			Mesh* current_mesh = new Mesh(imported_scene->mMeshes[i]);
			APPLOG("New mesh with %d vertices", imported_scene->mMeshes[i]->mNumVertices);
			return current_mesh;
		}
		aiReleaseImport(imported_scene);
	}
	else
		APPLOG("Error loading scene %s", file);

	return current_mesh;
}