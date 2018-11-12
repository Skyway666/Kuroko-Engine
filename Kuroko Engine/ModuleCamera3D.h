#ifndef _MODULE_CAMERA
#define _MODULE_CAMERA
#include "Module.h"
#include "Globals.h"

#include "MathGeoLib\Math\float4x4.h"

#include <list>

class Camera;


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();

public:
	Camera* editor_camera = nullptr;
	Camera* current_camera = nullptr;
	Camera* selected_camera = nullptr;
	std::list<Camera*> game_cameras;
};

#endif