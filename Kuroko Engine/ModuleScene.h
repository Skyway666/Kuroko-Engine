#ifndef __MODULE_SCENE
#define __MODULE_SCENE
#include "Module.h"
#include "MathGeoLib\MathGeoLib.h"
#include <list>

class GameObject; 
class Material;
class Mesh;
class Texture;

enum GameState { STOPPED, PAUSED, PLAYING};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Play() { game_state = PLAYING; };
	void Pause() { game_state = PAUSED; };
	void Stop() { game_state = STOPPED; };

	Material* getMaterial(uint id) const;
	GameObject* getGameObject(uint id) const;
	Mesh* getMesh(uint id) const;
	Texture* getTexture(uint id) const;

	void deleteMaterial(Material* mat) const;
	void deleteGameObject(GameObject* gobj) const;
	void deleteMesh(Mesh* mesh) const;
	void deleteTexture(Texture* tex) const;

private:
	
	void DrawGrid() const;

public:

	std::list<GameObject*> game_objects;
	std::list<GameObject*> game_objs_to_delete;
	std::list<Material*> materials;
	std::list<Material*> materials_to_delete;
	std::list<Mesh*> meshes;
	std::list<Mesh*> meshes_to_delete;
	std::list<Texture*> textures;
	std::list<Texture*> textures_to_delete;

	GameObject* selected_obj	= nullptr;
	bool draw_grid				= true;
	GameState game_state		= STOPPED;
	uint last_mat_id			= 0;
	uint last_gobj_id			= 0;
	uint last_mesh_id			= 0;
	uint last_tex_id			= 0;

	bool global_wireframe		= false;
	bool global_normals			= false;
};
#endif