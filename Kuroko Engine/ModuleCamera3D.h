#ifndef _MODULE_CAMERA
#define _MODULE_CAMERA
#include "Module.h"
#include "Globals.h"

#include "MathGeoLib\Math\float4x4.h"

#include <list>

class Texture;
class Camera;

struct FrameBuffer
{
	Texture* tex = nullptr;
	Texture* depth_tex = nullptr;
	uint id = 0;
	uint depth_test_id = 0;

	uint size_x = 0;
	uint size_y = 0;
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();
	FrameBuffer* initFrameBuffer();

public:
	Camera* editor_camera = nullptr;
	Camera* current_camera = nullptr;
	Camera* selected_camera = nullptr;
	std::list<Camera*> game_cameras;

	std::list<FrameBuffer*> frame_buffers;
	std::list<FrameBuffer*> frame_buffers_to_delete;
};

#endif