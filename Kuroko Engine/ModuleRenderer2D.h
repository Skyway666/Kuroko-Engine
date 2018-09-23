#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "SDL\include\SDL.h"

class ModuleRenderer2D: public Module {
public:
	ModuleRenderer2D(Application* app, bool start_enabled = true);
	~ModuleRenderer2D();

	bool Init(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SetViewportSize(int display_x, int display_y);
public:

	SDL_Renderer* renderer;
	SDL_Rect camera;
};

