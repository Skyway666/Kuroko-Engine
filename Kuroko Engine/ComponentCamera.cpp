#include "ComponentCamera.h"
#include "Camera.h"
#include "ComponentTransform.h"
#include "GameObject.h"


ComponentCamera::ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, CAMERA), camera(camera)
{
	transform = (ComponentTransform*)parent->getComponent(TRANSFORM);
}

bool ComponentCamera::Update(float dt)
{
	float4x4 inh_transform = transform->getInheritedTransform();
	camera->getFrustum()->pos = inh_transform.TranslatePart() + offset;
	float3 euler_angles = inh_transform.RotatePart().ToEulerXYZ(); 
	float3 look_towards = float3::unitZ * (camera->Reference - camera->getFrustum()->pos).Length();

	if (!lock_rotationX)	look_towards = Quat::RotateX(euler_angles.x) * look_towards;
	if (!lock_rotationY)	look_towards = Quat::RotateY(euler_angles.y) * look_towards;
	if (!lock_rotationZ)	look_towards = Quat::RotateZ(euler_angles.z) * look_towards;

	camera->LookAt(look_towards);
	
	return true;
}