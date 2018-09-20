#pragma once

#include "Component.h"
#include "Vector3.h"
#include "MathGeoLib\MathGeoLib.h"


class ComponentTransform : public Component {
public:

	friend class ModuleImGUI;

	ComponentTransform(GameObject* parent, Quat rot, Vector3f pos, Vector3f scl) : position(pos), scale(scl), rotation(rot), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, Vector3f euler_axis, Vector3f pos, Vector3f scl) : position(pos), scale(scl), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, ComponentTransform& transform);
	ComponentTransform(GameObject* parent) : Component(parent, TRANSFORM) { rotation = Quat::identity; };

	~ComponentTransform() {};

	float4x4 getInheritedTransform(Vector3f pos = Vector3f::Zero, Quat rot = { 0.0f ,0.0f ,0.0f, 1.0f }, Vector3f scl = Vector3f::One);  // DON'T set values to the args

	void setPosition(Vector3f pos);
	void Translate(Vector3f dir);
	void setScale(Vector3f scl);
	void Scale(Vector3f scl);
	void setRotation(Quat rot) { rotation = rot; euler_angles = rot.ToEulerXYZ(); };
	void setRotationEuler(Vector3f euler);
	void RotateAroundAxis(Vector3f axis, float rot_in_degrees) { rotation.RotateAxisAngle(axis.toMathVec(), DegToRad(rot_in_degrees)); euler_angles = rotation.ToEulerXYZ();};
	float4x4 getModelViewMatrix();

	void LookAt(Vector3f position, Vector3f target, Vector3f forward, Vector3f up);

	Vector3f getPosition() { return position; };
	Vector3f getScale() { return scale; };
	Vector3f getRotationEuler() { return Vector3f(euler_angles.x, euler_angles.y, euler_angles.z); };
	Quat getRotation() { return rotation; };

	float3 Forward();
	float3 Right();
	float3 Up();


private:

	Vector3f position = { 0.0f,0.0f,0.0f };			
	Vector3f scale = { 1.0f,1.0f,1.0f };				
	Quat rotation = { 0.0f,0.0f,0.0f,1.0f };	

	float3 euler_angles = { 0.0f,0.0f,0.0f };

	float4x4 mat = mat.identity;
};
