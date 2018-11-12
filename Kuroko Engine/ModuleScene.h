#ifndef __MODULE_SCENE
#define __MODULE_SCENE
#include "Module.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\Geometry\Frustum.h"
#include "ImGui\ImGuizmo.h"
#include <list>

class GameObject; 
class Material;
class Mesh;
class Texture;
class Skybox;
class Quadtree;

struct RayHit
{
	RayHit(float distance, GameObject*obj) : distance(distance), obj(obj) {};
	float distance = 0.0f;
	GameObject* obj = nullptr;
};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PostUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void DrawScene(float3 camera_pos);

	void addMaterial(Material* mat)			{ materials.push_back(mat); };
	void addGameObject(GameObject* gobj)	{ game_objects.push_back(gobj); };
	void addMesh(Mesh* mesh)				{ meshes.push_back(mesh); };
	void addTexture(Texture* tex)			{ textures.push_back(tex); };

	Material* getMaterial(uint id) const;
	GameObject* getGameObject(uint id) const;
	Mesh* getMesh(uint id) const;
	Texture* getTexture(uint id) const;
	bool existingScene() { return working_on_existing_scene;}
	std::string getWorkigSceneName() { return current_working_scene; }

	void deleteMaterial(Material* mat)		{ materials_to_delete.push_back(mat); };
	void deleteGameObject(GameObject* gobj)	{ game_objs_to_delete.push_back(gobj); };
	void deleteGameObjectRecursive(GameObject* gobj);
	void deleteMesh(Mesh* mesh)				{ meshes_to_delete.push_back(mesh); };
	void deleteTexture(Texture* tex)		{ textures_to_delete.push_back(tex); };
	void ClearScene();

	void getRootObjs(std::list<GameObject*>& list_to_fill);


	void AskPrefabLoadFile(char* path);
	void AskSceneSaveFile(char* scene_name); 
	void AskSceneLoadFile(char* path);
	void AskLocalSaveScene() { want_local_save = true; }
	void AskLocalLoadScene() { want_local_load = true; }


	GameObject* MousePicking(float x, float y, GameObject* ignore = nullptr);
private:
	
	void ManageSceneSaveLoad();
	void SaveScene(std::string name);
	void LoadScene(const char* path);
	void LoadPrefab(const char* path);
	JSON_Value* serializeScene();
	void loadSerializedScene(JSON_Value* scene);
private:

	std::list<GameObject*>	game_objects; 
	std::list<Material*>	materials;
	std::list<Mesh*>		meshes;
	std::list<Texture*>		textures;

	std::list<GameObject*>	game_objs_to_delete;
	std::list<Material*>	materials_to_delete;
	std::list<Mesh*>		meshes_to_delete;
	std::list<Texture*>		textures_to_delete;

	Skybox* skybox			= nullptr;
	Quadtree * quadtree		= nullptr;

	bool want_save_scene_file = false;
	bool want_load_scene_file = false;
	bool want_load_prefab_file = false;
	bool want_local_save      = false;
	bool want_local_load	  = false;

	bool working_on_existing_scene	  = false;

	std::string current_working_scene = ""; // "" means that no scene is being edited

	std::string scene_to_save_name;
	std::string path_to_load_scene;
	std::string path_to_load_prefab;

	JSON_Value* local_scene_save = nullptr;		// To use when time starts and resumes

public:

	GameObject* selected_obj	= nullptr;

	uint last_mat_id			= 0;
	uint last_gobj_id			= 0;
	uint last_mesh_id			= 0;
	uint last_tex_id			= 0;

	bool global_wireframe		= false;
	bool global_normals			= false;
	// Dirty bools related to quadtee
	bool draw_quadtree			= true;
	bool quadtree_reload		= false;
	GameObject* quadtree_add	= nullptr;

};
#endif