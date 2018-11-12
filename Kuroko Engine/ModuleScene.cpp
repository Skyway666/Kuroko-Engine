#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ImGui\imgui.h"
#include "Material.h"
#include "Mesh.h"
#include "ModuleInput.h"
#include "GameObject.h"		
#include "Camera.h"
#include "Skybox.h"
#include "Quadtree.h"
#include "ModuleRenderer3D.h"
#include "Applog.h"
#include "FileSystem.h"
#include "ModuleDebug.h"
#include "ModuleWindow.h"
#include "ComponentAABB.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "ComponentMesh.h"
#include "ModuleUI.h"

#include "ModuleImporter.h" // TODO: remove this include and set skybox creation in another module (Importer?, delayed until user input?)
#include "MathGeoLib\Geometry\LineSegment.h"
#include "glew-2.1.0\include\GL\glew.h"

#include "ImGui\imgui.h"

#include <array>
#include <map>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene(){}

// Load assets
bool ModuleScene::Start()
{
	skybox = new Skybox();
	want_save_scene_file = false;
	want_load_scene_file = false;

	std::array<Texture*, 6> skybox_texs;
	skybox_texs[LEFT]	= (Texture*)App->importer->Import("Assets/Textures/skybox_default_left.png", I_TEXTURE);
	skybox_texs[RIGHT]	= (Texture*)App->importer->Import("Assets/Textures/skybox_default_right.png", I_TEXTURE);
	skybox_texs[UP]		= (Texture*)App->importer->Import("Assets/Textures/skybox_default_up.png", I_TEXTURE);
	skybox_texs[DOWN]	= (Texture*)App->importer->Import("Assets/Textures/skybox_default_down.png", I_TEXTURE);
	skybox_texs[FRONT]	= (Texture*)App->importer->Import("Assets/Textures/skybox_default_front.png", I_TEXTURE);
	skybox_texs[BACK]	= (Texture*)App->importer->Import("Assets/Textures/skybox_default_back.png", I_TEXTURE);
	skybox->setAllTextures(skybox_texs);

	// TEST FOR QUADTREE
	quadtree = new Quadtree(AABB(float3(-50, -10, -50), float3(50, 10, 50)));
	// TEST FOR QUADTREE
	return true;
}

// Load assets
bool ModuleScene::CleanUp()
{
	for (auto it = game_objects.begin(); it != game_objects.end();it++)
		delete *it;

	for (auto it = materials.begin(); it != materials.end(); it++)
		delete *it;

	for (auto it = meshes.begin(); it != meshes.end(); it++)
		delete *it;

	for (auto it = textures.begin(); it != textures.end(); it++)
		delete *it;

	game_objects.clear();
	materials.clear();
	meshes.clear();
	textures.clear();

	game_objs_to_delete.clear();
	materials_to_delete.clear();
	meshes_to_delete.clear();
	textures_to_delete.clear();

	delete skybox;
	delete quadtree;

	if (local_scene_save) {
		json_value_free(local_scene_save);
	}

	selected_obj = nullptr;
	return true;
}

update_status ModuleScene::PostUpdate(float dt)
{
	// Quadtree management
	if(draw_quadtree)
		quadtree->DebugDraw();

	if (quadtree_add) {
		quadtree->Insert(quadtree_add);
		quadtree_add = nullptr;
	}
	if (quadtree_reload) {
		quadtree->Empty();
		for (auto it = game_objects.begin(); it != game_objects.end(); it++)
			if ((*it)->isStatic()) 
				quadtree->Insert(*it);

		quadtree_reload = false;
	}
	


	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
	{
		//If something is deleted, ask quadtree to reload
		quadtree_reload = true;
		if (*it == selected_obj) selected_obj = nullptr;
		game_objects.remove(*it);

		if (GameObject* parent = (*it)->getParent())
			parent->removeChild(*it);

		delete *it;
	}

	for (auto it = materials_to_delete.begin(); it != materials_to_delete.end(); it++)
	{
		materials.remove(*it);
		delete *it;
	}

	for (auto it = textures_to_delete.begin(); it != textures_to_delete.end(); it++)
	{
		textures.remove(*it);
		delete *it;
	}

	for (auto it = meshes_to_delete.begin(); it != meshes_to_delete.end(); it++)
	{
		meshes.remove(*it);
		delete *it;
	}

	game_objs_to_delete.clear();
	materials_to_delete.clear();
	textures_to_delete.clear();
	meshes_to_delete.clear();

	ManageSceneSaveLoad();

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update(float dt)
{
	if (!ImGui::IsMouseHoveringAnyWindow() && App->input->GetMouseButton(1) == KEY_DOWN && !ImGuizmo::IsOver() && App->camera->selected_camera == App->camera->editor_camera)
	{
		float x = (((App->input->GetMouseX() / (float)App->window->main_window->width) * 2) - 1);
		float y = (((((float)App->window->main_window->height - (float)App->input->GetMouseY()) / (float)App->window->main_window->height) * 2) - 1);

		if (GameObject* new_selected = MousePicking(x, y))
			selected_obj = new_selected;
	}

	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		(*it)->Update(dt);

	return UPDATE_CONTINUE;
}

void ModuleScene::DrawScene(float3 camera_pos)
{
	if (skybox)
	{
		skybox->updatePosition(camera_pos);
		skybox->Draw();
	}

	App->debug->DrawShapes();

	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		(*it)->Draw();
}

bool sortCloserRayhit(const RayHit& a, const RayHit& b) { return a.distance < b.distance; }

GameObject* ModuleScene::MousePicking(float x, float y, GameObject* ignore)
{
	GameObject* ret = nullptr;

	Frustum* f = App->camera->selected_camera->getFrustum();
	Ray ray = f->UnProjectLineSegment(x , y).ToRay();

	std::list<GameObject*> intersected_objs;

	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
	{
		if ((*it)->getComponent(MESH))
		{
			OBB* obb = ((ComponentAABB*)(*it)->getComponent(C_AABB))->getOBB();
			if (ray.Intersects(*obb) && *it != ignore)
				intersected_objs.push_back(*it);
		}
	}
	
	if (intersected_objs.empty())
		return ret;

	std::list<RayHit> ray_hits;
	
	for (auto it = intersected_objs.begin(); it != intersected_objs.end(); it++)
	{
		Transform* global = ((ComponentTransform*)(*it)->getComponent(TRANSFORM))->global;
		std::list<Component*> meshes;
		(*it)->getComponents(MESH, meshes);
		for (auto it2 = meshes.begin(); it2 != meshes.end(); it2++)
		{
			Mesh* mesh = ((ComponentMesh*)*it2)->getMesh();

			for (int i = 0; i < mesh->getNumTris(); i++)
			{
				const float3* vertices = mesh->getVertices();
				const Tri* tris = mesh->getTris();

				Triangle t((global->getRotation() * (global->getScale().Mul(vertices[tris[i].v1])) + global->getPosition()),
					(global->getRotation() * (global->getScale().Mul(vertices[tris[i].v2])) + global->getPosition()),
					(global->getRotation() * (global->getScale().Mul(vertices[tris[i].v3])) + global->getPosition()));

				float out_dist = 0.0f;
				float3 intersection_point = float3::zero;
				if (ray.Intersects(t, &out_dist, &intersection_point))
					ray_hits.push_back(RayHit(out_dist, *it));
			}
		}
	}

	if (ray_hits.empty())
		return ret;
	else {
		ray_hits.sort(sortCloserRayhit);
		return ray_hits.front().obj;
	}
}
Material* ModuleScene::getMaterial(uint id)  const
{
	for (auto it = materials_to_delete.begin(); it != materials_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = materials.begin(); it != materials.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}

GameObject* ModuleScene::getGameObject(uint id) const
{
	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
		if (id == (*it)->getId())
			return *it;

	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}
Mesh* ModuleScene::getMesh(uint id) const
{
	for (auto it = meshes_to_delete.begin(); it != meshes_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = meshes.begin(); it != meshes.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}
Texture* ModuleScene::getTexture(uint id) const
{
	for (auto it = textures_to_delete.begin(); it != textures_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = textures.begin(); it != textures.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}

void ModuleScene::ClearScene()
{
	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		delete *it;

	game_objects.clear();

	selected_obj = nullptr;
}

void ModuleScene::getRootObjs(std::list<GameObject*>& list_to_fill)
{
	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		if (!(*it)->getParent())
			list_to_fill.push_back(*it);

	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
		for (auto it2 = list_to_fill.begin(); it2 != list_to_fill.end(); it2++)
			if (*it == *it2)
				list_to_fill.remove(*it);
}

void ModuleScene::deleteGameObjectRecursive(GameObject* gobj)
{
	game_objs_to_delete.push_back(gobj);

	std::list<GameObject*> children;
	gobj->getChildren(children);

	for (auto it = children.begin(); it != children.end(); it++)
		deleteGameObjectRecursive(*it);
}

void ModuleScene::AskPrefabLoadFile(char * path) {

}

void ModuleScene::AskSceneSaveFile(char * scene_name) {
	want_save_scene_file = true;
	scene_to_save_name = scene_name;
}

void ModuleScene::AskSceneLoadFile(char * path) {
	want_load_scene_file = true;
	path_to_load_scene = path;
}


void ModuleScene::ManageSceneSaveLoad() {
	if (want_save_scene_file) {
		SaveScene(scene_to_save_name);
		want_save_scene_file = false;
	}
	if (want_load_scene_file) {
		LoadScene(path_to_load_scene.c_str());
		want_load_scene_file = false;
	}

	if (want_load_prefab_file) {
		LoadPrefab(path_to_load_prefab.c_str());
		want_load_prefab_file = false;
	}
	if (want_local_save) {
		local_scene_save = serializeScene();
		want_local_save = false;
	}
	if (want_local_load) {
		if (local_scene_save) {
			loadSerializedScene(local_scene_save);
			json_value_free(local_scene_save);
			local_scene_save = nullptr;
		}
		else
			app_log->AddLog("Couldn't load locally serialized scene, value was NULL");

		want_local_load = false;
	}
}

void ModuleScene::SaveScene(std::string name) {

	if (App->fs->ExistisFile(name.c_str(), ASSETS_SCENES, JSON_EXTENSION))
		app_log->AddLog("%s scene already created, overwritting...", name.c_str());

	App->fs->CreateEmptyFile(name.c_str(), ASSETS_SCENES, JSON_EXTENSION);

	current_working_scene = name;
	working_on_existing_scene = true;

	JSON_Value* scene = serializeScene();

	std::string path;
	App->fs->FormFullPath(path, name.c_str(), ASSETS_SCENES, JSON_EXTENSION);
	json_serialize_to_file(scene, path.c_str());

	// Free everything
	json_value_free(scene);
}

void ModuleScene::LoadScene(const char* path) {
	JSON_Value* scene = json_parse_file(path);
	if (!scene) {
		app_log->AddLog("Couldn't load %s, no value", path);
		return;
	}

	// Store current working scene
	std::string name = path;
	App->fs->getFileNameFromPath(name);
	current_working_scene = name;
	working_on_existing_scene = true;
	ClearScene();
	quadtree_reload = true;
	loadSerializedScene(scene);
	json_value_free(scene);
}

void ModuleScene::LoadPrefab(const char * path) {

}

JSON_Value * ModuleScene::serializeScene() {

	JSON_Value* scene = json_value_init_object();	// Full file object
	JSON_Value* objects_array = json_value_init_array();	// Array of objects in the scene

	json_object_set_value(json_object(scene), "Game Objects", objects_array); // Add array to file

	for (auto it = game_objects.begin(); it != game_objects.end(); it++) {
		JSON_Value* object = json_value_init_object();	// Object in the array
		(*it)->Save(json_object(object));				// Fill content
		json_array_append_value(json_array(objects_array), object); // Add object to array
	}

	// TODO store editor camera(?)

	return scene;
}

void ModuleScene::loadSerializedScene(JSON_Value * scene) {

	JSON_Array* objects = json_object_get_array(json_object(scene), "Game Objects");

	uint obj_num = json_array_get_count(objects);
	uint* parents = new uint[obj_num];  // Allocate all the parents in an array
										// Load all the objects and put them in the scene array

	for (int i = 0; i < json_array_get_count(objects); i++) {
		JSON_Object* obj_deff = json_array_get_object(objects, i);
		GameObject* obj = new GameObject(obj_deff);
		parents[i] = json_object_get_number(obj_deff, "Parent");  // Put the UUID of the parent in the same position as the child
		uint uuid = json_object_get_number(obj_deff, "UUID");
		game_objects.push_back(obj);
	}

	int i = 0;
	for (auto it = game_objects.begin(); it != game_objects.end(); it++) {
		if (parents[i] != 0) {			// If the UUID of the parent is 0, it means that it has no parent
			GameObject* child = (*it);
			GameObject* parent = nullptr;
			// Look for the parent among all gameobjects
			for (auto pos_par = game_objects.begin(); pos_par != game_objects.end(); pos_par++) {
				if ((*pos_par)->getUUID() == parents[i]) {	// We find the parent
					parent = (*pos_par);
					break;									// Only one parent/child, no need to keep looping
				}
			}
			// Link parent and child
			if(parent){
			child->setParent(parent);
			parent->addChild(child);
			}
		}
		i++;
	}

	delete parents;
}



