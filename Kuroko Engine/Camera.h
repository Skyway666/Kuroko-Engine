#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "MathGeoLib\Math\float4x4.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Geometry\Frustum.h"
#include "MathGeoLib\Geometry\OBB.h"

class FrameBuffer;

#define EXTRA_DIST 3.0f

class Camera
{
	friend class ModuleCamera3D;

public:
	Camera(float3 position = float3::zero, float3 reference = float3(0.0f, 0.0f, 5.0f), float n_plane = 0.125f, float f_plane = 1250.0f, float hor_fov = 90.0f, float ver_fov = 59.0f);
	~Camera() {};

	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	void FitToSizeSelectedGeometry(float extra_distance = EXTRA_DIST);
	void LookAtSelectedGeometry();
	void Reset();

	Frustum* getFrustum() const { return frustum; };
	FrameBuffer* getFrameBuffer() const { return frame_buffer; };

	void setFrameBuffer(FrameBuffer* fb) { frame_buffer = fb; };
	void setFrustum(Frustum* f) { frustum = f; };
	void setFov(float hor_fov, float ver_fov);
	void setPlaneDistance(float n_plane, float f_plane);
	bool frustumCull(const OBB& obb);

	float4x4 getViewMatrix();

private:

	void updateFrustum();

public:

	float3 X = { 1.0f,0.0f,0.0f };
	float3 Y = { 0.0f,1.0f,0.0f };
	float3 Z = { 0.0f,0.0f,1.0f };
	float3 Reference = { 0.0f,0.0f,5.0f };

private:

	FrameBuffer* frame_buffer = nullptr;
	Frustum* frustum = nullptr;
};


#endif
