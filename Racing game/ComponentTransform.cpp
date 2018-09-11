#include "ComponentTransform.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"


ComponentTransform::ComponentTransform(GameObject* parent, ComponentTransform& transform) : Component(parent, TRANSFORM)
{
	position = transform.position; rotation = transform.rotation; scale = transform.scale;
}

float4x4 ComponentTransform::getInheritedTransform()
{
	GameObject* obj = getParent();
	if (GameObject* parent_obj = obj->getParent())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)parent_obj->getComponent(TRANSFORM);
		return getModelViewMatrix() * parent_transform->getInheritedTransform();
	}
	else
		return getModelViewMatrix();
}



void ComponentTransform::setRotationEuler(Vector3f euler) 
{
	rotation = Quat::identity;
	rotation = rotation * rotation.RotateX(DegToRad(euler.x));
	rotation = rotation * rotation.RotateY(DegToRad(euler.y));
	rotation = rotation * rotation.RotateZ(DegToRad(euler.z));
	euler_angles = euler.toMathVec();
}

void ComponentTransform::LookAt(Vector3f position, Vector3f target, Vector3f forward, Vector3f up)
{
	mat = mat.LookAt(position.toMathVec(), target.toMathVec(), forward.toMathVec(), up.toMathVec(), position.Up.toMathVec());
	rotation = mat.RotatePart().ToQuat();
}

float4x4 ComponentTransform::getModelViewMatrix()
{
	mat = float4x4::identity;
	mat = mat * rotation;
	mat = mat * mat.Scale(scale.toMathVec());
	mat.SetTranslatePart(position.toMathVec());
	return mat;
}


float3 ComponentTransform::Forward()
{
	float3 forward = { 0.0f, 0.0f, 1.0f };
	forward = rotation * forward;
	return forward;
}

float3 ComponentTransform::Up()
{
	float3 up = { 0.0f, 1.0f, 0.0f };
	up = rotation * up;
	return up;
}

float3 ComponentTransform::Right()
{
	float3 right = { 1.0f, 0.0f, 0.0f };
	right = rotation * right;
	return right;
}