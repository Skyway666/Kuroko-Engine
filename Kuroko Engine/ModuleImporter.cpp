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
#include "Camera.h"
#include "FileSystem.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "Timer.h"
#include "Random.h"

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


#include "ModuleCamera3D.h"

ModuleImporter::ModuleImporter(Application* app, bool start_enabled) : Module(app, start_enabled) {
	name = "importer";
}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(const JSON_Object* config)
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
	std::string extension = file;
	App->fs->getExtension(extension);
	Timer load_time;
	load_time.Start();

	if (expected_filetype == I_NONE || expected_filetype == I_GOBJ)
	{
		if (extension == ".FBX" || extension == ".fbx" || extension == ".dae" || extension == ".blend" || extension == ".3ds" || extension == ".obj"
			|| extension == ".gltf" || extension == ".glb" || extension == ".dxf" || extension == ".x")
		{
			const aiScene* imported_scene = aiImportFile(file, aiProcessPreset_TargetRealtime_MaxQuality);

			if (imported_scene)
			{
				std::vector<uint> mat_id;
				//LoadMaterials(*imported_scene, mat_id);
				GameObject* root_obj = LoadNodeRecursive(*imported_scene->mRootNode, *imported_scene, mat_id);
				aiReleaseImport(imported_scene);

				App->scene->selected_obj = root_obj;
				App->camera->editor_camera->FitToSizeSelectedGeometry(); 
				app_log->AddLog("Success loading file: %s in %i ms", file, load_time.Read());

				return root_obj;
			}
			else
				app_log->AddLog("Error loading scene %s", file);
		}
		// Import own file format
		if (extension == ".kr"){
			Mesh* mesh = ImportMeshFromKR(file);
			std::string mesh_name = file;
			App->fs->getFileNameFromPath(mesh_name);
			mesh->setName(mesh_name.c_str());
			if (mesh) {
				GameObject* mesh_object = new GameObject("mesh_loaded_from_kr");
				ComponentMesh* c_mesh = new ComponentMesh(mesh_object, mesh);
				mesh_object->addComponent(c_mesh);
				app_log->AddLog("Success loading file: %s in %i ms", file, load_time.Read());
			}

		}
	}
	if (expected_filetype == I_NONE || expected_filetype == I_TEXTURE)
	{
		if (extension == ".bmp" || extension == ".dds" || extension == ".jpg" || extension == ".pcx" || extension == ".png"
			|| extension == ".raw" || extension == ".tga" || extension == ".tiff")
		{
			std::string texture_name = file;
			App->fs->getFileNameFromPath(texture_name);
			bool is_dds = extension == ".dds";
			Texture* tex = new Texture(ilutGLLoadImage((char*)file), texture_name.c_str(), !is_dds);  // If it is a dds, don't compress it

			if (is_dds) { 									// We copy and paste file in library folder.
				if (!App->fs->copyFileTo(file, LIBRARY_TEXTURES, DDS_EXTENSION))
					app_log->AddLog("%s could not be copied to Library/Textures", file);
			}

			ILinfo ImageInfo;
			iluGetImageInfo(&ImageInfo);
			if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
				iluFlipImage();


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

GameObject* ModuleImporter::LoadNodeRecursive(const aiNode& node, const aiScene& scene, const std::vector<uint>& in_mat_id, GameObject* parent)
{
	GameObject* root_obj = new GameObject(node.mName.C_Str(), parent);

	for (int i = 0; i < node.mNumMeshes; i++)
	{
		Mesh* mesh = new Mesh(*scene.mMeshes[node.mMeshes[i]], node.mName.C_Str());
		ComponentMesh* c_m = new ComponentMesh(root_obj, mesh);
		if (scene.mMeshes[node.mMeshes[i]]->mMaterialIndex < in_mat_id.size())
			c_m->setMaterial(App->scene->getMaterial(in_mat_id.at(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex)));
		else {
			Material* mat = new Material();
			c_m->setMaterial(mat);
		}
		root_obj->addComponent(c_m);

		ExportMeshToKR(node.mName.C_Str(), mesh);
		app_log->AddLog("New mesh with %d vertices", scene.mMeshes[node.mMeshes[i]]->mNumVertices);
	}

	aiVector3D pos = { 0.0f, 0.0f, 0.0f };
	aiVector3D scl = { 1.0f, 1.0f, 1.0f };;
	aiQuaternion rot;
	node.mTransformation.Decompose(scl, rot, pos);

	((ComponentTransform*)root_obj->getComponent(TRANSFORM))->local->Set(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.x, rot.w), float3(scl.x, scl.y, scl.z));

	for (int i = 0; i < node.mNumChildren; i++)
		root_obj->addChild(LoadNodeRecursive(*node.mChildren[i], scene, in_mat_id, root_obj));

	return root_obj;
}

void ModuleImporter::ImportNodeToSceneRecursive(const aiNode & node, const aiScene & scene, JSON_Value * objects_array) {

	JSON_Value* game_object = json_value_init_object();
	JSON_Value* components = json_value_init_array();

	json_object_set_value(json_object(game_object), "Components", components);
	for (int i = 0; i < node.mNumMeshes; i++) {
		// Import, store and delete mesh
		Mesh* mesh = new Mesh(*scene.mMeshes[node.mMeshes[i]], node.mName.C_Str());
		JSON_Value* mesh_component = json_value_init_object();	
		uint uuid_number = random32bits();																// Create mesh component
		std::string uuid = std::to_string(uuid_number);
		std::string binary_full_path = MESHES_FOLDER + uuid + ENGINE_EXTENSION;
		json_object_set_string(json_object(mesh_component), "type", "mesh");			// Set type
		json_object_set_string(json_object(mesh_component), "mesh binary", binary_full_path.c_str()); // Add mesh 
		json_object_set_number(json_object(mesh_component), "uuid", uuid_number);
		// TODO: Add material of the mesh												// Add material
		//if (scene.mMeshes[node.mMeshes[i]]->mMaterialIndex < in_mat_id.size())
		//	material = in_mat_id.at(scene.mMeshes[node.mMeshes[i]]->mMaterialIndex);
		json_array_append_value(json_array(components), mesh_component);			// Add component to components
		ExportMeshToKR(uuid.c_str(), mesh);				// Import mesh
		//delete mesh;									// TODO: Delete mesh

		app_log->AddLog("Imported mesh with %i vertices", scene.mMeshes[node.mMeshes[i]]->mNumVertices);
	}

	// Import and store transform
	aiVector3D pos = { 0.0f, 0.0f, 0.0f };
	aiVector3D scl = { 1.0f, 1.0f, 1.0f };;
	aiQuaternion rot;
	node.mTransformation.Decompose(scl, rot, pos);
	Transform* trans = new Transform();
	JSON_Value* transform_component = json_value_init_object();
	JSON_Value* local_transform = json_value_init_object();
	trans->Set(float3(pos.x, pos.y, pos.z), Quat(rot.x, rot.y, rot.x, rot.w), float3(scl.x, scl.y, scl.z));
	trans->Save(json_object(local_transform));
	json_object_set_string(json_object(transform_component), "type", "transform"); // Set type
	json_object_set_value(json_object(transform_component), "local transform", local_transform); //Set local transform
	json_array_append_value(json_array(components), transform_component);			// Add component to components
	delete trans;

	json_array_append_value(json_array(objects_array), game_object);    // Add gameobject to gameobject array


	for (int i = 0; i < node.mNumChildren; i++)
		ImportNodeToSceneRecursive(*node.mChildren[i], scene, objects_array);

}

void ModuleImporter::ImportMaterialsFromNode(const aiScene & scene, std::vector<JSON_Object*>& out_mat_id)
{
	aiString path;
	for (int i = 0; i < scene.mNumMaterials; i++)
	{
		JSON_Object* material = json_object(json_value_init_object());
		out_mat_id.push_back(material);
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE))
		{
			scene.mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			// Find the path in assets (or not), ManageFile, then add the uuid to the material
		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_AMBIENT))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_AMBIENT, 0, &path);
			// Find the path in library (or not), ManageFile, then add the binary to the material

		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_NORMALS))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &path);
			// Find the path in library (or not), ManageFile, then add the binary to the material

		}
		if (scene.mMaterials[i]->GetTextureCount(aiTextureType_LIGHTMAP))
		{
			path.Clear();
			scene.mMaterials[i]->GetTexture(aiTextureType_LIGHTMAP, 0, &path);
			// Find the path in library (or not), ManageFile, then add the binary to the material

		}
	}
}

bool ModuleImporter::ImportScene(const char * file_original_name, std::string file_binary_name) {

	const aiScene* imported_scene = aiImportFile(file_original_name, aiProcessPreset_TargetRealtime_MaxQuality);

	if (imported_scene) {
		JSON_Value* scene = json_value_init_object();
		JSON_Value* objects_array = json_value_init_array();	// Array of objects in the scene
		//std::vector<uint> mat_id;
		//LoadMaterials(*imported_scene, mat_id);

		json_object_set_value(json_object(scene), "Game Objects", objects_array); // Add array to file

		//std::vector<JSON_Object*> out_serializedMat_id;
		//ImportMaterialsFromNode(*imported_scene, out_serializedMat_id);
		ImportNodeToSceneRecursive(*imported_scene->mRootNode, *imported_scene, objects_array);

		std::string path;
		App->fs->FormFullPath(path, file_binary_name.c_str(), LIBRARY_PREFABS, JSON_EXTENSION);
		json_serialize_to_file(scene, path.c_str());
		json_value_free(scene);
		aiReleaseImport(imported_scene);
		app_log->AddLog("Success importing file %s (scene)", file_original_name);

		return true;
	}
	else
		app_log->AddLog("Error importing file %s (scene): imported scene was null", file_original_name);

	return false;
}
void logAssimp(const char* message, char* user) {
	app_log->AddLog("%s", message);
}

bool ModuleImporter::ImportTexture(const char * file_original_name, std::string file_binary_name) {

	std::string path, name, extension;
	path = name = extension = file_original_name;
	App->fs->getExtension(extension);
	App->fs->getPath(path);
	App->fs->getFileNameFromPath(name);
	bool is_dds = extension == ".dds";
	Texture* tex = new Texture(ilutGLLoadImage((char*)file_original_name), file_binary_name.c_str(), !is_dds);  // If it is a dds, don't compress it

	if (is_dds) { 									// We copy and paste file in library folder.
		if (!App->fs->copyFileTo(file_original_name, LIBRARY_TEXTURES, DDS_EXTENSION, file_binary_name))
			app_log->AddLog("%s could not be copied to Library/Textures", file_original_name);
	}

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		iluFlipImage();


	// TODO: delete texture and not add it to scene list (will be done when resources are fully working

	app_log->AddLog("Success importing texture: %s", file_original_name);
	return tex;
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
	//if (has_normals)
		header[2] = vert_num;
	//else
	//	header[2] = 0;
	//if (has_colors)
		header[3] = vert_num;
	//else
	//	header[3] = 0;
	//if (has_texcoords)
		header[4] = vert_num;
	//else
	//	header[4] = 0;

	// Knowing the size of the file, we can create the buffer in which it all will be stored
	uint size = sizeof(header) + sizeof(float3)*vert_num + sizeof(Tri)*poly_count;
	//if (has_normals)
		size += sizeof(float3)*vert_num;
	//if (has_colors)
		size += sizeof(float3)*vert_num;
	//if (has_texcoords)
		size += sizeof(float2)*vert_num;

	char* data = new char[size];
	char* cursor = data;

	// Get the data
	const float3* vertices = mesh->getVertices();
	const Tri* tris = mesh->getTris();
	const float3* normals = mesh->getNormals();
	const float3* colors = mesh->getColors();
	const float2* tex_coords = mesh->getTexCoords();

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
	//if (has_normals) {
		bytes = sizeof(float3)*vert_num;
		memcpy(cursor, normals, bytes);
		cursor += bytes;
	//}

	// Colors
	//if (has_colors) {
		bytes = sizeof(float3)*vert_num;
		memcpy(cursor, colors, bytes);
		cursor += bytes;
	//}

	// Tex coords
	//if (has_texcoords) {
		bytes = sizeof(float2)*vert_num;
		memcpy(cursor, tex_coords, bytes);
	//}
	
	std::string filename = file;
	App->fs->getFileNameFromPath(filename);
	App->fs->ExportBuffer(data, size, filename.c_str(), LIBRARY_MESHES, ENGINE_EXTENSION);
	app_log->AddLog("Saved %s as own file format", filename.c_str());

	delete data;
}

void ModuleImporter::ExportTextureToDDS(const char* texture_name) {
	ILuint size;
	char *data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {
		data = new char[size]; // allocate data buffer
		if (ilSaveL(IL_DDS, data, size) > 0) // Save to buffer with the ilSaveIL function
			App->fs->ExportBuffer(data, size, texture_name, LIBRARY_TEXTURES, DDS_EXTENSION);
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
	if (!App->fs->ExistisFile(file)) {
		app_log->AddLog("Couldn't load mesh, not found in library");
		return nullptr;
	}


	char* buffer = App->fs->ImportFile(file);
	char* cursor = buffer;

	// Read the header
	uint header[5];
	uint bytes = sizeof(header);
	memcpy(header, cursor, bytes);
	num_vertices = header[0];
	num_tris = header[1];
	//if (header[2] > 0)
	//	imported_normals = true;
	//if (header[3] > 0)
	//	imported_colors = true;
	//if (header[4] > 0)
	//	imported_tex_coords = true;
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
	//if(imported_normals){
	// Read tris
	bytes = sizeof(float3)*num_vertices;
	normals = new float3[num_vertices];
	memcpy(normals, cursor, bytes);
	// Increment cursor
	cursor += bytes;
	//}

	// Read colors (if any)
	//if (imported_colors) {
		// Read tris
		bytes = sizeof(float3)*num_vertices;
		colors = new float3[num_vertices];
		memcpy(colors, cursor, bytes);
		// Increment cursor
		cursor += bytes;
	//}

	// Read tex_coords (if any)
	//if (imported_tex_coords) {
		// Read tris
		bytes = sizeof(float2)*num_vertices;
		tex_coords = new float2[num_vertices];
		memcpy(tex_coords, cursor, bytes);
	//}
	app_log->AddLog("Loaded mesh %s from own file format", file);
	return new Mesh(vertices,tris,normals,colors,tex_coords, num_vertices, num_tris);
}

Texture * ModuleImporter::LoadTextureFromLibrary(const char * file) {
	return new Texture(ilutGLLoadImage((char*)file), file, false);
}


