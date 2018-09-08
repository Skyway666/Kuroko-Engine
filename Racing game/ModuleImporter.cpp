#include "ModuleImporter.h"
#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "Material.h"

#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"

#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#pragma comment (lib, "DevIL/lib/x86/Release/DevIL.lib")
#pragma comment (lib, "DevIL/lib/x86/Release/ILU.lib")
#pragma comment (lib, "DevIL/lib/x86/Release/ILUT.lib")


ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) {}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	return true;
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	return true;
}

GameObject* ModuleImporter::LoadFBX(const char* file)
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

GameObject* ModuleImporter::LoadAssimpNode(aiNode* node, const aiScene* scene, GameObject* parent)
{
	GameObject* root_obj = new GameObject(node->mName.C_Str(), parent);

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		root_obj->addComponent(new ComponentMesh(root_obj, scene->mMeshes[node->mMeshes[i]]));
		APPLOG("New mesh with %d vertices", scene->mMeshes[node->mMeshes[i]]->mNumVertices);
	}

	for (int i = 0; i < node->mNumChildren; i++)
		root_obj->addChild(LoadAssimpNode(node->mChildren[i], scene, root_obj));

	return root_obj;
}

bool ModuleImporter::LoadRootMesh(const char* file, ComponentMesh* component_to_load)
{
	const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (imported_scene && imported_scene->HasMeshes())
	{
		component_to_load->ClearData();
		if (component_to_load->loaded = component_to_load->LoadFromAssimpMesh(imported_scene->mMeshes[0]))
			component_to_load->LoadDataToVRAM();
		aiReleaseImport(imported_scene);
		return true;
	}
	else
		APPLOG("Error loading scene %s", file);

	return false;
}

Material* ModuleImporter::quickLoadTex(char* file)
{
	return new Material(ilutGLLoadImage(file));
}

Material* ModuleImporter::LoadTex(char* file, Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter)
{
	Material* mat = new Material(ilutGLLoadImage(file));
	mat->setParameters(wrap, min_filter, mag_filter);
	return mat;
}