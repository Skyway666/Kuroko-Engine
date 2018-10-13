#ifndef __ModuleRenderer_H__
#define __ModuleRenderer_H__

#include "Module.h"
#include "Globals.h"
#include "Light.h"

#include "MathGeoLib\Math\float3x3.h"
#include "MathGeoLib\Math\float4x4.h"

#include <list>

#define MAX_LIGHTS 8

class Texture;

struct FrameBuffer
{
	Texture* tex = nullptr;
	uint id = 0;
	uint depthbuffer_id = 0;

	uint size_x = 0;
	uint size_y = 0;
};

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init(const JSON_Object& config);
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	SDL_GLContext getContext() const	{ return context; }

	void DirectDrawCube(float3& size, float3& pos) const;
	FrameBuffer* initFrameBuffer(uint size_x = 1024, uint size_y = 768);

private:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
public:
	std::list<FrameBuffer*> frame_buffers;
	std::list<FrameBuffer*> frame_buffers_to_delete;
};

#endif