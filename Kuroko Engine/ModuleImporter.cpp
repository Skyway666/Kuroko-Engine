#include "ModuleImporter.h"
#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "Material.h"
#include "ModuleImGUI.h"
#include "ModuleSceneIntro.h"
#include "Application.h"
#include "Applog.h"
#include "Assimp\include\DefaultLogger.hpp"

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <fstream>
#include <string>


#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
#include "Assimp\include\Logger.hpp"

#include "DevIL/include/IL/il.h"
#include "DevIL/include/IL/ilu.h"
#include "DevIL/include/IL/ilut.h"

#pragma comment (lib, "Assimp/lib/assimp.lib")

#pragma comment (lib, "DevIL/lib/DevIL.lib")
#pragma comment (lib, "DevIL/lib/ILU.lib")
#pragma comment (lib, "DevIL/lib/ILUT.lib")


ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) {
	name = "importer";
}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(JSON_Object* config)
{

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "log.txt");
	aiAttachLogStream(&stream);


	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);

	ilEnable(IL_CONV_PAL);
	ilutEnable(ILUT_OPENGL_CONV);

	return true;
}

bool ModuleImporter::CleanUp()
{
	aiDetachAllLogStreams();
	if (checkered_tex)
		delete checkered_tex;
	return true;
}

GameObject* ModuleImporter::LoadFBX(const char* file)
{
	GameObject* root_obj = nullptr;
	const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

	if (imported_scene)
	{
		
		std::vector<uint> mat_id;
		LoadMaterials(imported_scene, mat_id);
		root_obj = LoadMeshRecursive(imported_scene->mRootNode, imported_scene, mat_id);
		aiReleaseImport(imported_scene);
		
		// Read file and log info
		std::ifstream file_stream;
		std::string file_content;
		file_stream.open("log.txt");
		while (std::getline(file_stream, file_content))
			app_log->AddLog("%s", file_content.c_str());
		file_stream.close();
		//Clear the file 
		std::ofstream ofs;
		ofs.open("log.txt", std::ofstream::trunc);
		ofs.close();
	}
	else
		app_log->AddLog("Error loading scene %s", file);

	return root_obj;
}

uint ModuleImporter::LoadMaterials(const aiScene* scene, std::vector<uint>& out_mat_id)
{
	aiString path;
	for (int i = 0; i < scene->mNumMaterials; i++)
	{
		Material* new_mat = new Material();
		if (scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
		{
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			new_mat->setTexture(DIFFUSE, LoadTex((char*)path.C_Str()));
		}
		if (scene->mMaterials[i]->GetTextureCount(aiTextureType_AMBIENT))
		{
			path.Clear();
			scene->mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &path);
			new_mat->setTexture(AMBIENT, LoadTex((char*)path.C_Str()));
		}
		if (scene->mMaterials[i]->GetTextureCount(aiTextureType_NORMALS))
		{
			path.Clear();
			scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &path);
			new_mat->setTexture(NORMALS, LoadTex((char*)path.C_Str()));
		}
		if (scene->mMaterials[i]->GetTextureCount(aiTextureType_LIGHTMAP))
		{
			path.Clear();
			scene->mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &path);
			new_mat->setTexture(LIGHTMAP, LoadTex((char*)path.C_Str()));
		}
		out_mat_id.push_back(new_mat->getId());
		App->scene_intro->materials.push_back(new_mat);
	}

	return scene->mNumMaterials;
}

GameObject* ModuleImporter::LoadMeshRecursive(aiNode* node, const aiScene* scene, const std::vector<uint>& in_mat_id, GameObject* parent)
{
	GameObject* root_obj = new GameObject(node->mName.C_Str(), parent);

	for (int i = 0; i < node->mNumMeshes; i++)
	{
		ComponentMesh* mesh = new ComponentMesh(root_obj, scene->mMeshes[node->mMeshes[i]]);
		mesh->mat = App->scene_intro->getMaterial(in_mat_id.at(scene->mMeshes[node->mMeshes[i]]->mMaterialIndex));
		root_obj->addComponent(mesh);
		app_log->AddLog("New mesh with %d vertices", scene->mMeshes[node->mMeshes[i]]->mNumVertices);
	}

	for (int i = 0; i < node->mNumChildren; i++)
		root_obj->addChild(LoadMeshRecursive(node->mChildren[i], scene, in_mat_id, root_obj));

	return root_obj;
}


Texture* ModuleImporter::LoadTex(char* file, Mat_Wrap wrap, Mat_MinMagFilter min_filter, Mat_MinMagFilter mag_filter)
{
	Texture* tex = new Texture(ilutGLLoadImage(file));
	tex->setParameters(wrap, min_filter, mag_filter);
	return tex;
}

