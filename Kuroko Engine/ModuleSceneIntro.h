#pragma once
#include "Module.h"
#include "MathGeoLib\MathGeoLib.h"
#include <list>

class GameObject; 

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	void Play() {};
	void Pause() {};
	void Stop() {};

	std::list<GameObject*> game_objects;
	GameObject* selected_obj = nullptr;
	bool draw_grid = true;

private:
	
	void DrawGrid();
};
