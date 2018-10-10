#ifndef _COMPONENT_TRANSFORM
#define _COMPONENT_TRANSFORM

#include "Component.h"
#include "MathGeoLib\MathGeoLib.h"


class ComponentTransform : public Component {
public:

	friend class ModuleImGUI;

	ComponentTransform(GameObject* parent, Quat rot, float3 pos, float3 scl) : position(pos), scale(scl), rotation(rot), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, float3 euler_axis, float3 pos, float3 scl) : position(pos), scale(scl), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, ComponentTransform& transform);
	ComponentTransform(GameObject* parent) : Component(parent, TRANSFORM) { rotation = Quat::identity; };

	~ComponentTransform() {};

	float4x4 getInheritedTransform(float3 pos = float3::zero, Quat rot = { 0.0f ,0.0f ,0.0f, 1.0f }, float3 scl = float3::one) const;  // DON'T set values to the args

	void setPosition(float3 pos);
	void Translate(float3 dir);
	void setScale(float3 scl);
	void Scale(float3 scl);
	void setRotation(Quat rot) { rotation = rot; euler_angles = rot.ToEulerXYZ(); };
	void setRotationEuler(float3 euler);
	void RotateAroundAxis(float3 axis, float rot_in_degrees) { rotation.RotateAxisAngle(axis, DegToRad(rot_in_degrees)); euler_angles = rotation.ToEulerXYZ(); };
	float4x4 getModelViewMatrix();

	void LookAt(float3 position, float3 target, float3 forward, float3 up);


	float3 getPosition()const { return position; };
	float3 getScale() const { return scale; };
	float3 getRotationEuler()const { return float3(euler_angles.x, euler_angles.y, euler_angles.z); };
	Quat getRotation()const { return rotation; };

	float3 Forward() const;
	float3 Right()const;
	float3 Up()const;


private:

	float3 position = { 0.0f,0.0f,0.0f };
	float3 scale = { 1.0f,1.0f,1.0f };
	Quat rotation = { 0.0f,0.0f,0.0f,1.0f };

	float3 euler_angles = { 0.0f,0.0f,0.0f };

	float4x4 mat = mat.identity;
};
#endif