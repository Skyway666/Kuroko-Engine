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

	friend class ModuleImGUI;
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void Play() { game_state = PLAYING; };
	void Pause() { game_state = PAUSED; };
	void Stop() { game_state = STOPPED; };
	GameState getGameState() { return game_state; };

	void addMaterial(Material* mat)			{ materials.push_back(mat); };
	void addGameObject(GameObject* gobj)	{ game_objects.push_back(gobj); };
	void addMesh(Mesh* mesh)				{ meshes.push_back(mesh); };
	void addTexture(Texture* tex)			{ textures.push_back(tex); };

	Material* getMaterial(uint id) const;
	GameObject* getGameObject(uint id) const;
	Mesh* getMesh(uint id) const;
	Texture* getTexture(uint id) const;

	void deleteMaterial(Material* mat)		{ materials_to_delete.push_back(mat); };
	void deleteGameObject(GameObject* gobj)	{ game_objs_to_delete.push_back(gobj); };
	void deleteMesh(Mesh* mesh)				{ meshes_to_delete.push_back(mesh); };
	void deleteTexture(Texture* tex)		{ textures_to_delete.push_back(tex); };

private:
	
	void DrawGrid() const;

private:

	std::list<GameObject*>	game_objects;
	std::list<Material*>	materials;
	std::list<Mesh*>		meshes;
	std::list<Texture*>		textures;

	std::list<GameObject*>	game_objs_to_delete;
	std::list<Material*>	materials_to_delete;
	std::list<Mesh*>		meshes_to_delete;
	std::list<Texture*>		textures_to_delete;

	GameState game_state	= STOPPED;
public:


	GameObject* selected_obj	= nullptr;
	bool draw_grid				= true;
	uint last_mat_id			= 0;
	uint last_gobj_id			= 0;
	uint last_mesh_id			= 0;
	uint last_tex_id			= 0;

	bool global_wireframe		= false;
	bool global_normals			= false;
};
#endif