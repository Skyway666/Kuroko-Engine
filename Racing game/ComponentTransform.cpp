#include "ComponentTransform.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"


ComponentTransform::ComponentTransform(GameObject* parent, Quat rot, Vector3f pos, Vector3f scl) : position(pos), scale(scl), rotation(rot), Component(parent, TRANSFORM)
{
	mat = mat.FromTRS(position.toMathVec(), rotation, scale.toMathVec());
}

ComponentTransform::ComponentTransform(GameObject* parent, Vector3f euler_axis, Vector3f pos, Vector3f scl) : position(pos), scale(scl), Component(parent, TRANSFORM)
{
	rotation = rotation.FromEulerXYZ(euler_axis.x, euler_axis.y, euler_axis.z);
	mat = mat.FromTRS(position.toMathVec(), rotation, scale.toMathVec());
}

ComponentTransform::ComponentTransform(GameObject* parent, ComponentTransform& transform) : Component(parent, TRANSFORM)
{
	position = transform.position; rotation = transform.rotation; scale = transform.scale;
	mat = transform.mat;
}

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent, TRANSFORM)
{
	mat = mat.FromTRS(position.toMathVec(), rotation, scale.toMathVec());
}


float4x4 ComponentTransform::getInheritedTransform()
{
	GameObject* obj = getParent();
	if (GameObject* parent_obj = obj->getParent())
	{
		ComponentTransform* parent_transform = (ComponentTransform*)parent_obj->getComponent(TRANSFORM);
		return mat * parent_transform->getInheritedTransform();
	}
	else
		return mat;
}

void ComponentTransform::SetPosition(Vector3f pos)
{
	mat = mat.FromTRS(pos.toMathVec(), rotation, scale.toMathVec());
	position = pos;
}

void ComponentTransform::Translate(Vector3f dir) 
{
	mat = mat.FromTRS((position + dir).toMathVec(), rotation, scale.toMathVec());
	position += dir;
}

void ComponentTransform::SetRotation(Quat rot)
{
	mat = mat.FromTRS(position.toMathVec(), rot, scale.toMathVec());
	rotation = rot;
}

void ComponentTransform::RotateAroundAxis(Vector3f axis, float rot_in_degrees)
{
	mat = mat.RotateAxisAngle(axis.toMathVec(), DegToRad(rot_in_degrees));
	float3 euler_axis = mat.ToEulerXYZ();
	rotation = rotation.FromEulerXYZ(euler_axis.x, euler_axis.y, euler_axis.z);
}

void ComponentTransform::SetRotationEuler(Vector3f euler_axis) 
{
	Quat rot = rot.FromEulerXYZ(euler_axis.x, euler_axis.y, euler_axis.z);
	mat = mat.FromTRS(position.toMathVec(), rot, scale.toMathVec());
	rotation = rot;
}

Vector3f ComponentTransform::getRotationEuler()
{
	float3 rotation = mat.ToEulerXYZ();
	return (Vector3f(rotation.x, rotation.y, rotation.z));
}

void ComponentTransform::SetScale(Vector3f scl)
{
	mat = mat.FromTRS(position.toMathVec(), rotation, scl.toMathVec());
	scale = scl;
}

void ComponentTransform::Scale(Vector3f scl) 
{
	mat = mat.FromTRS(position.toMathVec(), rotation, (scale + scl).toMathVec());
	scale += scl;
}

void ComponentTransform::LookAt(Vector3f position, Vector3f target, Vector3f forward, Vector3f up, Vector3f worldUp)
{
	mat = mat.LookAt(position.toMathVec(), target.toMathVec(), forward.toMathVec(), up.toMathVec(), worldUp.toMathVec());
	this->position = position;
	float3 euler_axis = mat.ToEulerXYZ();
	rotation = rotation.FromEulerXYZ(euler_axis.x, euler_axis.y, euler_axis.z);
}


