#include "ModuleImporter.h"
#include "GameObject.h"
#include "Globals.h"
#include "ComponentMesh.h"
#include "Material.h"
#include "ModuleUI.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Applog.h"
#include "ModuleAudio.h"
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

#include "shlwapi.h"

#pragma comment (lib, "Shlwapi.lib")

#pragma comment (lib, "Assimp/lib/assimp.lib")

#pragma comment (lib, "DevIL/lib/DevIL.lib")
#pragma comment (lib, "DevIL/lib/ILU.lib")
#pragma comment (lib, "DevIL/lib/ILUT.lib")


#include "ModuleCamera3D.h"

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) {
	name = "importer";
}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(const JSON_Object& config)
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
	return true;
}


void* ModuleImporter::Import(const char* file, ImportType expected_filetype) const
{
	std::string extension = PathFindExtensionA(file);

	if (expected_filetype == I_NONE || expected_filetype == I_GOBJ)
	{
		if (extension == ".FBX" || extension == ".fbx" || extension == ".dae" || extension == ".blend" || extension == ".3ds" || extension == ".obj"
			|| extension == ".gltf" || extension == ".glb" || extension == ".dxf" || extension == ".x")
		{
			const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

			if (imported_scene)
			{
				std::vector<uint> mat_id;
				LoadMaterials(*imported_scene, mat_id);
				GameObject* root_obj = LoadMeshRecursive(*imported_scene->mRootNode, *imported_scene, mat_id);
				aiReleaseImport(imported_scene);
				// Just for assignment 1
				if(!App->scene_intro->game_objects.empty())
					App->scene_intro->game_objs_to_delete.push_back(App->scene_intro->game_objects.front());

				App->scene_intro->game_objects.push_back(root_obj);
				App->camera->FocusSelectedGeometry(3); // Hardcoded value
				app_log->AddLog("Success loading file: %s", file);

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

				return root_obj;
			}
			else
				app_log->AddLog("Error loading scene %s", file);
		}
	}
	if (expected_filetype == I_NONE || expected_filetype == I_TEXTURE)
	{
		if (extension == ".bmp" || extension == ".dds" || extension == ".jpeg" || extension == ".pcx" || extension == ".png"
			|| extension == ".raw" || extension == ".tga" || extension == ".tiff")
		{
			Material* mat = new Material();
			Texture* tex = new Texture(ilutGLLoadImage((char*)file));
			mat->setTexture(DIFFUSE, tex);
			App->scene_intro->materials.push_back(mat);
			//Just for assignment 1
			if (!App->scene_intro->game_objects.empty()) {
				std::list<GameObject*>::iterator it = App->scene_intro->game_objects.end();
				it--;
				//Get last game object and assign the material if they have a mesh
				GameObject* object = *it;
				ComponentMesh* mesh = (ComponentMesh*)object->getComponent(MESH);
				if(mesh) mesh->setMaterial(mat);
				//Get children and assign the material if they have a mesh
				std::list<GameObject*> children;
				object->getChildren(children);
				for (auto it = children.begin(); it != children.end(); it++) {
					GameObject* object = *it;
					ComponentMesh* mesh = (ComponentMesh*)object->getComponent(MESH);
					if (mesh) mesh->setMaterial(mat);
				}
			}
			return tex;
			app_log->AddLog("Success loading texture: %s", file);
		}
	}/*
	if (expected_filetype == I_NONE || expected_filetype == I_MUSIC)
	{
		if (extension == ".mod" || extension == ".midi" || extension == ".mp3" || extension == ".flac")
		{
			std::string name = PathFindFileNameA(file);
			if (AudioFile* last_audio_file = App->audio->LoadAudio(file, name.c_str(), MUSIC))
			{
				app_log->AddLog("Success loading music: %s", file);
				return last_audio_file;
			}
			else 
				app_log->AddLog("Error loading music: %s", file);
		}
	}
	if (expected_filetype == I_NONE || expected_filetype == I_FX)
	{
		if (extension == ".wav" || extension == ".aiff" || extension == ".riff" || extension == ".ogg" || extension == ".voc")
		{
			std::string name = PathFindFileNameA(file);
			if (AudioFile* last_audio_file = App->audio->LoadAudio(file, name.c_str(), FX))
			{
				app_log->AddLog("Success loading fx: %s", file);
				return last_audio_file;
			}
			else
				app_log->AddLog("Error loading fx: %s", file);
		}
	}*/
	else
		app_log->AddLog("Error loading file [incompatible format]: %s", file);

	return nullptr;
}


bool ModuleImporter::LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const
{
	aiString path;
	for (int i = 0; i < scene.mNumMaterials; i++)
	{
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
		{
			scene.mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			if(App->importer->Import(path.C_Str(), I_TEXTURE))
				out_mat_id.push_back(App->scene_intro->materials.back()->getId());
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_AMBIENT))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &path);
			if (App->importer->Import(path.C_Str(), I_TEXTURE))
				out_mat_id.push_back(App->scene_intro->materials.back()->getId());
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_NORMALS))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &path);
			if (App->importer->Import(path.C_Str(), I_TEXTURE))
				out_mat_id.push_back(App->scene_intro->materials.back()->getId());
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_LIGHTMAP))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &path);
			if (App->importer->Import(path.C_Str(), I_TEXTURE))
				out_mat_id.push_back(App->scene_intro->materials.back()->getId());
		}
	}

	return scene.mNumMaterials == out_mat_id.size();
}

GameObject* ModuleImporter::LoadMeshRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent) const
{
	GameObject* root_obj = new GameObject(node.mName.C_Str(), parent);

	for (int i = 0; i < node.mNumMeshes; i++)
	{
		Mesh* mesh = new Mesh(*scene.mMeshes[node.mMeshes[i]]);
		ComponentMesh* c_m = new ComponentMesh(root_obj, mesh);
		if(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex < in_mat_id.size())
			c_m->setMaterial(App->scene_intro->getMaterial(in_mat_id.at(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex)));
		root_obj->addComponent(c_m);
		app_log->AddLog("New mesh with %d vertices", scene.mMeshes[node.mMeshes[i]]->mNumVertices);
	}

	for (int i = 0; i < node.mNumChildren; i++)
		root_obj->addChild(LoadMeshRecursive(*node.mChildren[i], scene, in_mat_id, root_obj));

	return root_obj;
}

