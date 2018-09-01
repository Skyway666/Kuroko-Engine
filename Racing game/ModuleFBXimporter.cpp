#include "ModuleFBXimporter.h"
#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"

#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")



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

GameObject* ModuleFBXimporter::LoadFBX(const char* file)
{
	GameObject* root_obj = nullptr;
	const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (imported_scene)
	{
		root_obj = LoadAssimpNode(imported_scene->mRootNode, imported_scene);
		aiReleaseImport(imported_scene);
	}
	else
		APPLOG("Error loading scene %s", file);

	return root_obj;
}

GameObject* ModuleFBXimporter::LoadAssimpNode(aiNode* node, const aiScene* scene, GameObject* parent)
{
	GameObject* root_obj = new GameObject(node->mName.C_Str());

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		root_obj->addComponent(new ComponentMesh(root_obj, scene->mMeshes[node->mMeshes[i]]));
		APPLOG("New mesh with %d vertices", scene->mMeshes[node->mMeshes[i]]->mNumVertices);
	}

	for (int i = 0; i < node->mNumChildren; i++)
		root_obj->addChild(LoadAssimpNode(node->mChildren[i], scene, root_obj));

	return root_obj;
}

bool ModuleFBXimporter::LoadRootMesh(const char* file, ComponentMesh* component_to_load)
{
	const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (imported_scene && imported_scene->HasMeshes())
	{
		component_to_load->LoadFromAssimpMesh(imported_scene->mMeshes[0]);
		aiReleaseImport(imported_scene);
		return true;
	}
	else
		APPLOG("Error loading scene %s", file);

	return false;
}