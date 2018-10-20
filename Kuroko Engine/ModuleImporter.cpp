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
#include "FileSystem.h"

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
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, "log.txt");
	stream.callback = logAssimp;
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


void* ModuleImporter::Import(const char* file, ImportType expected_filetype) 
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

				App->scene->selected_obj = root_obj;
				App->camera->editor_camera->FitToSizeSelectedGeometry(); 
				app_log->AddLog("Success loading file: %s", file);

				return root_obj;
			}
			else
				app_log->AddLog("Error loading scene %s", file);
		}
		// Import own file format
		if (extension == ".kr"){
			Mesh* mesh = ImportMeshFromKR(file);
			if (mesh) {
				GameObject* mesh_object = new GameObject("mesh_loaded_from_kr");
				ComponentMesh* c_mesh = new ComponentMesh(mesh_object, mesh);
				mesh_object->addComponent(c_mesh);
			}

		}
	}
	if (expected_filetype == I_NONE || expected_filetype == I_TEXTURE)
	{
		if (extension == ".bmp" || extension == ".dds" || extension == ".jpg" || extension == ".pcx" || extension == ".png"
			|| extension == ".raw" || extension == ".tga" || extension == ".tiff")
		{
			Texture* tex = new Texture(ilutGLLoadImage((char*)file));
			app_log->AddLog("Success loading texture: %s", file);
			return tex;
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



void ModuleImporter::LoadMaterials(const aiScene& scene, std::vector<uint>& out_mat_id) const
{
	aiString path;
	for (int i = 0; i < scene.mNumMaterials; i++)
	{
		Material* mat = new Material();
		out_mat_id.push_back(App->scene->last_mat_id - 1);
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
		{
			scene.mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			if (Texture* tex = (Texture*)App->importer->Import(path.C_Str(), I_TEXTURE))
				mat->setTexture(DIFFUSE, tex);
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_AMBIENT))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &path);
			if (Texture* tex = (Texture*)App->importer->Import(path.C_Str(), I_TEXTURE))
				mat->setTexture(AMBIENT, tex);
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_NORMALS))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &path);
			if (Texture* tex = (Texture*)App->importer->Import(path.C_Str(), I_TEXTURE))
				mat->setTexture(NORMALS, tex);
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_LIGHTMAP))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &path);
			if (Texture* tex = (Texture*)App->importer->Import(path.C_Str(), I_TEXTURE))
				mat->setTexture(LIGHTMAP, tex);
		}
	}

}

GameObject* ModuleImporter::LoadMeshRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent) 
{
	GameObject* root_obj = new GameObject(node.mName.C_Str(), parent);

	for (int i = 0; i < node.mNumMeshes; i++)
	{
		Mesh* mesh = new Mesh(*scene.mMeshes[node.mMeshes[i]]);
		ComponentMesh* c_m = new ComponentMesh(root_obj, mesh);
		if(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex < in_mat_id.size())
			c_m->setMaterial(App->scene->getMaterial(in_mat_id.at(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex)));
		else {
			Material* mat = new Material();
			c_m->setMaterial(mat);
		}
		root_obj->addComponent(c_m);
		ExportMeshToKR(node.mName.C_Str(), mesh);
		app_log->AddLog("New mesh with %d vertices", scene.mMeshes[node.mMeshes[i]]->mNumVertices);
	}

	for (int i = 0; i < node.mNumChildren; i++)
		root_obj->addChild(LoadMeshRecursive(*node.mChildren[i], scene, in_mat_id, root_obj));

	return root_obj;
}

void logAssimp(const char* message, char* user) {
	app_log->AddLog("%s", message);
}

void ModuleImporter::ExportMeshToKR(const char * file, Mesh* mesh) {
	//Create a header for vertices, tris, normals, colors and tex coords
	uint header[5]; 
	uint vert_num, poly_count = 0;
	bool has_normals, has_colors, has_texcoords = false;

	// Get the data
	mesh->getData(vert_num, poly_count, has_normals, has_colors, has_texcoords);

	// Fill the header with data
	header[0] = vert_num;
	header[1] = poly_count;
	if (has_normals)
		header[2] = vert_num;
	else
		header[2] = 0;
	if (has_colors)
		header[3] = vert_num;
	else
		header[3] = 0;
	if (has_texcoords)
		header[4] = vert_num;
	else
		header[4] = 0;

	// Knowing the size of the file, we can create the buffer in which it all will be stored
	uint size = sizeof(header) + sizeof(float3)*vert_num + sizeof(Tri)*poly_count;
	if (has_normals)
		size += sizeof(float3)*vert_num;
	if (has_colors)
		size += sizeof(float3)*vert_num;
	if (has_texcoords)
		size += sizeof(float2)*vert_num;
	char* data = new char[size];
	char* cursor = data;

	// Get the data
	float3* vertices = nullptr;
	Tri* tris = nullptr;
	float3* normals = nullptr;
	float3* colors = nullptr;
	float2* tex_coords = nullptr;
	mesh->getNumbers(vertices, tris, normals, colors, tex_coords);

	// First we store the header
	uint bytes = sizeof(header);
	memcpy(cursor, header, bytes);
	cursor += bytes;

	// Vertices
	bytes = sizeof(float3)*vert_num;
	memcpy(cursor, vertices, bytes);
	cursor += bytes;

	// Tris
	bytes = sizeof(Tri)*poly_count;
	memcpy(cursor, tris, bytes);
	cursor += bytes;

	// Normals
	if (has_normals) {
		bytes = sizeof(float3)*vert_num;
		memcpy(cursor, normals, bytes);
		cursor += bytes;
	}

	// Colors
	if (has_colors) {
		bytes = sizeof(float3)*vert_num;
		memcpy(cursor, colors, bytes);
		cursor += bytes;
	}

	// Tex coords
	if (has_texcoords) {
		bytes = sizeof(float2)*vert_num;
		memcpy(cursor, tex_coords, bytes);
	}
	
	std::string filename = file;
	getFileNameFromPath(filename);
	filename.append(".kr"); // This is the file extension of this engine TODO: Store it in a variable
	App->fs->ExportBuffer(data, size, filename.c_str(), LIBRARY_MESHES);
	app_log->AddLog("Saved %s as own file format", filename.c_str()); // TODO: Tell in which folder was it loaded

	delete data;
}

void ModuleImporter::ExportTextureToDDS() {
	ILuint size;
	char *data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new char[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			App->fs->ExportBuffer(data, size, "texture.dds");
		delete data; 
	}
}

Mesh * ModuleImporter::ImportMeshFromKR(const char * file)
{
	uint num_vertices = 0;
	uint num_tris = 0;
	bool imported_normals = false;
	bool imported_colors = false;
	bool imported_tex_coords = false;

	float3* vertices = nullptr;
	Tri* tris = nullptr;
	float3* normals = nullptr;
	float3* colors = nullptr;
	float2* tex_coords = nullptr;

	// Import buffer from file
	char* buffer = App->fs->ImportFile(file);
	char* cursor = buffer;

	// Read the header
	uint header[5];
	uint bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	num_vertices = header[0];
	num_tris = header[1];
	if (header[2] > 0)
		imported_normals = true;
	if (header[3] > 0)
		imported_colors = true;
	if (header[4] > 0)
		imported_tex_coords = true;
	// Increment cursor
	cursor += bytes;

	// Read vertices
	bytes = sizeof(float3)*num_vertices;
	vertices = new float3[num_vertices];
	memcpy(vertices, cursor, bytes);
	// Increment cursor
	cursor += bytes;

	// Read tris
	bytes = sizeof(Tri)*num_tris;
	tris = new Tri[num_tris];
	memcpy(tris, cursor, bytes);
	// Increment cursor
	cursor += bytes;

	// Read normals (if any)
	if(imported_normals){
	// Read tris
	bytes = sizeof(float3)*num_vertices;
	normals = new float3[num_vertices];
	memcpy(normals, cursor, bytes);
	// Increment cursor
	cursor += bytes;
	}

	// Read colors (if any)
	if (imported_colors) {
		// Read tris
		bytes = sizeof(float3)*num_vertices;
		colors = new float3[num_vertices];
		memcpy(colors, cursor, bytes);
		// Increment cursor
		cursor += bytes;
	}

	// Read tex_coords (if any)
	if (imported_tex_coords) {
		// Read tris
		bytes = sizeof(float2)*num_vertices;
		tex_coords = new float2[num_vertices];
		memcpy(tex_coords, cursor, bytes);
	}
	app_log->AddLog("Loaded mesh %s from own file format", file);
	return new Mesh(vertices,tris,normals,colors,tex_coords, num_vertices, num_tris);
}

bool ModuleImporter::removeExtension(std::string& str) {
	size_t lastdot = str.find_last_of(".");
	if (lastdot == std::string::npos)
		return false;
	str = str.substr(0, lastdot);
	return true;
}

bool ModuleImporter::removePath(std::string& str) {
	const size_t last_slash_idx = str.find_last_of("\\/");
	if (last_slash_idx == std::string::npos ) 
		return false;
	str = str.erase(0, last_slash_idx + 1);
	return true;
}

void ModuleImporter::getFileNameFromPath(std::string & str) {
	removeExtension(str);
	removePath(str);
}
