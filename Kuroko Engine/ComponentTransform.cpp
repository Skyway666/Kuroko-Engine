#include "ComponentTransform.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"


ComponentTransform::ComponentTransform(GameObject* parent, const ComponentTransform& transform) : Component(parent, TRANSFORM)
{
	position = transform.position; rotation = transform.rotation; scale = transform.scale;
}

float4x4 ComponentTransform::getInheritedTransform(float3 pos, Quat rot, float3 scl) const 
{
	pos += position;
	scl.x *= scale.x; scl.y *= scale.y; scl.z *= scale.z;
	rot = rotation * rot;

	GameObject* obj = getParent();
	if (GameObject* parent_obj = obj->getParent())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)parent_obj->getComponent(TRANSFORM);
		return parent_transform->getInheritedTransform(pos, rot, scl);
	}
	else
	{
		float4x4 inh_mat = float4x4::identity;
		inh_mat = inh_mat * rot;
		inh_mat = inh_mat * inh_mat.Scale(scl);
		inh_mat.SetTranslatePart(pos);
		return inh_mat;
	}
}



void ComponentTransform::setRotationEuler(const float3& euler)
{
	rotation = Quat::identity;
	rotation = rotation * rotation.RotateX(DegToRad(euler.x));
	rotation = rotation * rotation.RotateY(DegToRad(euler.y));
	rotation = rotation * rotation.RotateZ(DegToRad(euler.z));
	euler_angles = euler;
}

void ComponentTransform::LookAt(const float3& position, const float3& target, const float3& forward, const float3& up)
{
	mat = mat.LookAt(position, target, forward, up, float3::unitY);
	rotation = mat.RotatePart().ToQuat();
}

float4x4 ComponentTransform::getModelViewMatrix() 
{
	mat = float4x4::identity;
	mat = mat * rotation;
	mat = mat * mat.Scale(scale);
	mat.SetTranslatePart(position);
	return mat;
}

float3 ComponentTransform::Right() const
{
	float3 right = float3::unitX;
	right = rotation * right;
	return right;
}

float3 ComponentTransform::Forward() const
{
	float3 forward = float3::unitZ;
	forward = rotation * forward;
	return forward;
}

float3 ComponentTransform::Up() const
{
	float3 up = float3::unitY;
	up = rotation * up;
	return up;
}

void ComponentTransform::Save(JSON_Object & config) {

}

void ComponentTransform::Load(JSON_Object & config) {

}


void ComponentTransform::setPosition(const float3& pos) { position = pos; };
void ComponentTransform::Translate(const float3& dir)	{ position += dir; };
void ComponentTransform::setScale(const float3& scl)	{ scale = scl; };
void ComponentTransform::Scale(const float3& scl)		{ scale.x *= scl.x; scale.y *= scl.y; scale.z *= scl.z;};