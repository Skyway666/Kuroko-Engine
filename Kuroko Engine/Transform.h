#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Globals.h"
#include "Parson\parson.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\Quat.h"
#include "MathGeoLib\Math\float4x4.h"

class Transform
{
	friend class ComponentTransform;
public:
	Transform() {}
	Transform(const Transform& transform);
	Transform(JSON_Object &deff);

	void Set(const float3& pos, const Quat& rot, const float3& scl) { setPosition(pos); setRotation(rot); setScale(scl); };
	void setPosition(const float3& pos);
	void Translate(const float3& dir);
	void setScale(const float3& scl);
	void Scale(const float3& scl);
	void setRotation(const Quat& rot) { rotation = rot; euler_angles = rot.ToEulerXYZ() * RADTODEG;; };
	void setRotationEuler(const float3& euler);
	void RotateAroundAxis(const float3& axis, float rot_in_degrees) { rotation.RotateAxisAngle(axis, DEGTORAD * (rot_in_degrees)); euler_angles = rotation.ToEulerXYZ() * RADTODEG; };
	void LookAt(const float3& position, const float3& target, const float3& forward, const float3& up);

	float4x4 CalculateMatrix();

	float3 getPosition()const { return position; };
	float3 getScale() const { return scale; };
	float3 getRotationEuler()const { return float3(euler_angles.x, euler_angles.y, euler_angles.z); };
	Quat getRotation()const { return rotation; };
	float4x4 getMatrix() const { return mat; };

	float3 Forward() const;
	float3 Right() const;
	float3 Up() const;

private:

	float3 position = float3::zero;
	float3 scale	= float3::one;
	Quat rotation	= Quat::identity;

	float3 euler_angles = float3::zero;

	float4x4 mat = mat.identity;
};

#endif