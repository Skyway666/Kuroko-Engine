#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Math\float3.h"

class FrameBuffer;

#define EXTRA_DIST 3.0f

class Camera
{
	friend class ModuleCamera3D;

public:
	Camera(float4x4 projection_matrix, float3 position = float3::zero, float3 reference = float3(0.0f, 0.0f, 5.0f));
	~Camera() {};

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	void FitToSizeSelectedGeometry(float extra_distance = EXTRA_DIST);
	void LookAtSelectedGeometry();
	void Reset();

	float* GetViewMatrix() const;
	float4x4 GetProjMatrix() const { return ProjectionMatrix; };
	void setProjMatrix(float4x4 mat) { ProjectionMatrix = mat; };

private:

	void CalculateViewMatrix();

public:

	float3 X = { 1.0f,0.0f,0.0f };
	float3 Y = { 0.0f,1.0f,0.0f };
	float3 Z = { 0.0f,0.0f,1.0f };
	float3 Position = { 0.0f,0.0f,0.0f };
	float3 Reference = { 0.0f,0.0f,5.0f };

	FrameBuffer* frame_buffer = nullptr;
private:

	float4x4 ViewMatrix = float4x4::identity;
	float4x4 ProjectionMatrix = float4x4::identity;
};


#endif
