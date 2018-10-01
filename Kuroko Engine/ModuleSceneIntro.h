#pragma once
#include "Module.h"
#include "MathGeoLib\MathGeoLib.h"
#include <list>

class GameObject; 

enum GameState { STOPPED, PAUSED, PLAYING};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:

	void Play() { game_state = PLAYING; };
	void Pause() { game_state = PAUSED; };
	void Stop() { game_state = STOPPED; };

	std::list<GameObject*> game_objects;
	GameObject* selected_obj = nullptr;
	bool draw_grid = true;
	GameState game_state = STOPPED;

private:
	
	void DrawGrid();
};
