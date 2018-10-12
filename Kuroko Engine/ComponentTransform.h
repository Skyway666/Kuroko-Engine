#ifndef _COMPONENT_TRANSFORM
#define _COMPONENT_TRANSFORM

#include "Component.h"
#include "MathGeoLib\MathGeoLib.h"


class ComponentTransform : public Component {
public:

	friend class ModuleUI;

	ComponentTransform(GameObject* parent, const Quat& rot, const float3& pos, const float3& scl) : position(pos), scale(scl), rotation(rot), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, const float3& euler_axis, const float3& pos, const float3& scl) : position(pos), scale(scl), Component(parent, TRANSFORM) {};
	ComponentTransform(GameObject* parent, const ComponentTransform& transform);
	ComponentTransform(GameObject* parent) : Component(parent, TRANSFORM) { rotation = Quat::identity; };

	~ComponentTransform() {};

	float4x4 getInheritedTransform(float3 pos = float3::zero, Quat rot = { 0.0f ,0.0f ,0.0f, 1.0f }, float3 scl = float3::one) const;  // args MUST be left empty!

	void setPosition(const float3& pos);
	void Translate(const float3& dir);
	void setScale(const float3& scl);
	void Scale(const float3& scl);
	void setRotation(const Quat& rot) { rotation = rot; euler_angles = rot.ToEulerXYZ(); };
	void setRotationEuler(const float3& euler);
	void RotateAroundAxis(const float3& axis, float rot_in_degrees) { rotation.RotateAxisAngle(axis, DegToRad(rot_in_degrees)); euler_angles = rotation.ToEulerXYZ(); };
	float4x4 getModelViewMatrix();

	void LookAt(const float3& position, const float3& target, const float3& forward, const float3& up);


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