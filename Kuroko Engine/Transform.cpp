#include "Transform.h"
#include "MathGeoLib\Math\TransformOps.h"

Transform::Transform(const Transform& transform)
{
	setPosition(transform.position);
	setRotation(transform.rotation);
	setScale(transform.scale);
	CalculateMatrix();
}

Transform::Transform(JSON_Object* deff) {

	setPosition(float3(json_object_get_number(deff, "px"), json_object_get_number(deff, "py"), json_object_get_number(deff, "pz")));
	setScale(float3(json_object_get_number(deff, "sx"), json_object_get_number(deff, "sy"), json_object_get_number(deff, "sz")));
	Quat rotation;
	rotation.x = json_object_get_number(deff, "qx");
	rotation.y = json_object_get_number(deff, "qy");
	rotation.z = json_object_get_number(deff, "qz");
	rotation.w = json_object_get_number(deff, "qw");
	setRotation(rotation);
}

void Transform::setRotationEuler(const float3& euler)
{
	rotation = Quat::identity;
	rotation = rotation * rotation.RotateX(DEGTORAD * (euler.x));
	rotation = rotation * rotation.RotateY(DEGTORAD * (euler.y));
	rotation = rotation * rotation.RotateZ(DEGTORAD * (euler.z));
	euler_angles = euler;
}

void Transform::LookAt(const float3& position, const float3& target, const float3& forward, const float3& up)
{
	mat = mat.LookAt(position, target, forward, up, float3::unitY);
	rotation = mat.RotatePart().ToQuat();
}

float4x4 Transform::CalculateMatrix()
{
	mat = float4x4::identity;
	mat = mat * rotation;
	mat = mat * mat.Scale(scale);
	mat.SetTranslatePart(position);
	return mat;
}

float3 Transform::Right() const
{
	float3 right = float3::unitX;
	right = rotation * right;
	return right;
}

float3 Transform::Forward() const
{
	float3 forward = float3::unitZ;
	forward = rotation * forward;
	return forward;
}

float3 Transform::Up() const
{
	float3 up = float3::unitY;
	up = rotation * up;
	return up;
}

void Transform::setPosition(const float3& pos) { position = pos; };
void Transform::Translate(const float3& dir) { position += dir; };
void Transform::setScale(const float3& scl) { scale = scl; };
void Transform::Scale(const float3& scl) { scale.x *= scl.x; scale.y *= scl.y; scale.z *= scl.z; };