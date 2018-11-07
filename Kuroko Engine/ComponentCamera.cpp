#include "ComponentCamera.h"
#include "Application.h"
#include "Camera.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"

#include "MathGeoLib\Math\Quat.h"

ComponentCamera::ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, CAMERA), camera(camera)
{
	transform = (ComponentTransform*)parent->getComponent(TRANSFORM);
	camera->attached_to = this;
}

bool ComponentCamera::Update(float dt)
{
	if (App->camera->selected_camera != camera)
	{
		float3 displacement = camera->getFrustum()->pos;
		camera->getFrustum()->pos = transform->local->getPosition() + offset;
		displacement = camera->getFrustum()->pos - displacement;
		camera->Reference += displacement;
		camera->Reference = camera->getFrustum()->pos + (transform->local->Forward().Normalized() * (camera->Reference - camera->getFrustum()->pos).Length());
		camera->X = transform->local->Right();
		camera->Y = transform->local->Up();
		camera->Z = transform->local->Forward();
		camera->updateFrustum();
	}
	/*
	float3 euler_angles = transform->local->getRotationEuler();
	float3 look_towards = float3::unitZ * (camera->Reference - camera->getFrustum()->pos).Length();

	if (!lock_rotationX)	look_towards = Quat::RotateX(euler_angles.x) * look_towards;
	if (!lock_rotationY)	look_towards = Quat::RotateY(euler_angles.y) * look_towards;
	if (!lock_rotationZ)	look_towards = Quat::RotateZ(euler_angles.z) * look_towards;

	camera->LookAt(look_towards);*/
	
	return true;
}