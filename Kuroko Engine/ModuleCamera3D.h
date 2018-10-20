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

	uint size_x = 0;
	uint size_y = 0;
};

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(const JSON_Object& config);
	update_status Update(float dt);
	bool CleanUp();
	float4x4 CreatePerspMat(float fov = 60.0f, float width = 1024.0f, float height = 768.0f, float near_plane = 0.125f, float far_plane = 1250.0f);
	FrameBuffer* initFrameBuffer(uint size_x = 1024, uint size_y = 768);

public:
	Camera* editor_camera = nullptr;
	std::list<Camera*> game_cameras;

	std::list<FrameBuffer*> frame_buffers;
	std::list<FrameBuffer*> frame_buffers_to_delete;
};

#endif