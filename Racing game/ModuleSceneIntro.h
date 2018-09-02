#pragma once
#include "Module.h"
#include <list>

class GameObject;  // <--  testing purposes

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();


public:

	std::list<GameObject*> game_objects;
	GameObject* selected_obj = nullptr;
};
