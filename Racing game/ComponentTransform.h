#pragma once

#include "Component.h"
#include "Vector3.h"
#include "MathGeoLib\MathGeoLib.h"


class ComponentTransform : Component {
public:

	ComponentTransform(GameObject* parent, Quat rot, Vector3f pos, Vector3f scl);
	ComponentTransform(GameObject* parent, Vector3f euler_axis, Vector3f pos, Vector3f scl);
	ComponentTransform(GameObject* parent, ComponentTransform& transform);
	ComponentTransform(GameObject* parent);

	~ComponentTransform() {};

	float4x4 getInheritedTransform();

	void SetPosition(Vector3f pos);
	void Translate(Vector3f dir);
	void SetScale(Vector3f scl);
	void Scale(Vector3f scl);
	void SetRotation(Quat rot);
	void SetRotationEuler(Vector3f euler_axis);
	void RotateAroundAxis(Vector3f axis, float rot_in_degrees);
	Vector3f getRotationEuler();

	void LookAt(Vector3f position, Vector3f target, Vector3f forward, Vector3f up, Vector3f worldUp);

public:

	Vector3f position = { 0.0f,0.0f,0.0f };			
	Vector3f scale = { 1.0f,1.0f,1.0f };				
	Quat rotation = { 0.0f,0.0f,0.0f,0.0f };			

	float4x4 mat = mat.identity;
};
