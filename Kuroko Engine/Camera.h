#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Math\float3.h"
#include "glmath.h"

class FrameBuffer;

#define EXTRA_DIST 3.0f

class Camera
{
	friend class ModuleCamera3D;

public:
	Camera(float4x4 projection_matrix, float3 position = float3(0.0f, 0.0f, -5.0f), float3 reference = float3::zero);
	~Camera() {};

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	void FitToSizeSelectedGeometry(float extra_distance = EXTRA_DIST);
	void LookAtSelectedGeometry();
	void Reset();

	float* GetViewMatrix() const;
	float4x4 GetProjMatrix() const { return ProjectionMatrix; };
	void setProjMatrix(float4x4 mat) { ProjectionMatrix = mat; };

private:

	void CalculateViewMatrix();

public:

	vec3 X = { 1.0f,0.0f,0.0f };
	vec3 Y = { 0.0f,1.0f,0.0f };
	vec3 Z = { 0.0f,0.0f,1.0f };
	vec3 Position = { 0.0f,0.0f,-5.0f };
	vec3 Reference = { 0.0f,0.0f,0.0f };

	FrameBuffer* frame_buffer = nullptr;
private:

	float4x4 ViewMatrix = float4x4::identity;
	float4x4 ProjectionMatrix = float4x4::identity;
};


#endif
