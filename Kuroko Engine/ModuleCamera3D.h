#ifndef _MODULE_CAMERA
#define _MODULE_CAMERA
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class vec3;

#define EXTRA_DIST 3.0f

#include "MathGeoLib\Math\float4x4.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(const JSON_Object& config);
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void FocusSelectedGeometry(float extra_distance = EXTRA_DIST);
	void RotateSelectedGeometry();
	void Reset();
	float* GetViewMatrix() const;

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;

private:
	
	float4x4 ViewMatrix, ViewMatrixInverse;
};
#endif