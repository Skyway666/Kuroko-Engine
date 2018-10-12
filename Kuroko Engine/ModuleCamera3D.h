#ifndef _MODULE_CAMERA
#define _MODULE_CAMERA
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

#define EXTRA_DIST 3.0f

#include "MathGeoLib\Math\float4x4.h"

#include <list>

class FrameBuffer;

class Camera
{
	friend class ModuleCamera3D;

public:
	Camera(float4x4 projection_matrix, float3 position = float3(0.0f, 0.0f, -5.0f), float3 reference = float3::zero);
	~Camera();
	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void FocusSelectedGeometry(float extra_distance = EXTRA_DIST);
	void RotateSelectedGeometry();
	void Reset();

	float* GetViewMatrix() const;
	float4x4 GetProjMatrix() const { return ProjectionMatrix; };
	void setProjMatrix(float4x4 mat) { ProjectionMatrix = mat; };

private:

	void CalculateViewMatrix();

public:

	vec3 X			= { 1.0f,0.0f,0.0f };
	vec3 Y			= { 0.0f,1.0f,0.0f };
	vec3 Z			= { 0.0f,0.0f,1.0f };
	vec3 Position	= { 0.0f,0.0f,-5.0f };
	vec3 Reference	= { 0.0f,0.0f,0.0f };

	FrameBuffer* frame_buffer = nullptr;
private:

	float4x4 ViewMatrix = float4x4::identity;
	float4x4 ProjectionMatrix = float4x4::identity;
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

public:

	Camera* editor_camera = nullptr;
	std::list<Camera*> game_cameras;
	float calculateModule(vec3);
};

#endif