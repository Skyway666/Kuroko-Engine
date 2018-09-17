#pragma once

#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\MathGeoLibFwd.h"

#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include <limits>

template <class TYPE>
class Vector3
{
public:
	TYPE x = 0.0f;
	TYPE y = 0.0f;
	TYPE z = 0.0f;

public:

	Vector3() {};
	Vector3(TYPE x, TYPE y, TYPE z) : x(x), y(y), z(z) {};
	Vector3(float3 vec) : x(vec.x), y(vec.y), z(vec.z) {};
	Vector3(btVector3 vec) : x(vec.x), y(vec.y), z(vec.z) {};
	Vector3(vec3 vec) : x(vec.x), y(vec.y), z(vec.z) {};


	float3	  toMathVec() { return float3((float)x, (float)y, (float)z); };
	btVector3 toBtVec() { return btVector3((float)x, (float)y, (float)z); };
	vec3	  toGlVec() { return vec3((float)x, (float)y, (float)z); };

	void setX(TYPE new_x) { x = new_x; };
	void setY(TYPE new_y) { y = new_y; };
	void setZ(TYPE new_z) { z = new_z; };

	void set(TYPE new_x, TYPE new_y, TYPE new_z) { x = new_x; y = new_y; z = new_z; };

	Vector3 operator -(const Vector3 &v)   const { return(Vector3(x - v.x, y - v.y, z - v.z)); };
	Vector3 operator +(const Vector3 &v)   const { return(Vector3(x + v.x, y + v.y, z + v.z)); };
	Vector3 operator *(const TYPE scalar) const { return(Vector3(x * scalar, y * scalar, z * scalar)); };

	bool operator ==(const Vector3& v) const { return (x == v.x && y == v.y && z == v.z); };
	bool operator !=(const Vector3& v) const { return (x != v.x || y != v.y || z != v.z); };

	const Vector3& operator -=(const Vector3 &v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return(*this);
	}

	const Vector3& operator +=(const Vector3 &v)
	{
		x += v.x; y += v.y; z += v.z;
		return(*this);
	}

	const Vector3& operator *=(const float &v)
	{
		x *= v; y *= v; z *= v;
		return(*this);
	}

	const Vector3& operator /=(const float &v)
	{
		x /= v; y /= v; z /= v;
		return(*this);
	}


	// Utils ------------------------------------------------

	bool IsZero() const { return (x == 0 && y == 0 && z == 0); };

	Vector3& SetToZero()
	{
		x = y = z = 0.0f;
		return(*this);
	}

	Vector3& Negate()
	{
		x = -x; y = -y; z = -z;
		return(*this);
	}

	//Module and angle, recommended to use with fpoints or ipoints
	float GetModule() const { return (float)sqrtf((x*x) + (y*y) + (z*z)); };

	// TODO: create getAngleX(), getAngleY(), getAngleZ()


	void Normalize()
	{
		if (!IsZero())
		{
			float module = GetModule();
			x = (x / module); y = (y / module); z = (z / module);
		}
	}

	Vector3 Normalized()
	{
		Vector3 Normalized = { 0.0f, 0.0f, 0.0f };

		if (!IsZero())
		{
			float module = GetModule();
			Normalized.x = (x / module); Normalized.y = (y / module); Normalized.z = (z / module);
		}

		return Normalized;
	}

	// Distances ---------------------------------------------

	float DistanceTo(const Vector3& v) const
	{
		float fx = x - v.x;
		float fy = y - v.y;
		float fz = z - v.z;

		return sqrtf((fx*fx) + (fy*fy) + (fz*fz));
	}

	void Rotate(Vector3<float> axis, int degrees)
	{
		btVector3 vec = toBtVec();
		vec = vec.rotate(axis.toBtVec(), degrees);
		x = vec.x(); y = vec.y(); z = vec.z();
	}

	void RotateEuler(float X, float Y, float Z)
	{
		btVector3 vec = toBtVec();
		vec = vec.rotate(Right.toBtVec(), X);
		vec = vec.rotate(Up.toBtVec(), Y);
		vec = vec.rotate(Forward.toBtVec(), Z);
		x = vec.x(); y = vec.y(); z = vec.z();
	}

	static Vector3<float> Up;
	static Vector3<float> Down;
	static Vector3<float> Right;
	static Vector3<float> Left;
	static Vector3<float> Forward;
	static Vector3<float> Back;
	static Vector3<float> Zero;
	static Vector3<float> One;
	static Vector3<float> PosInfinity;
	static Vector3<float> NegInfinity;

};

typedef Vector3<float> Vector3f;
typedef Vector3<float> Point3f;
typedef Vector3<int> Vector3i;
typedef Vector3<int> Point3i;
typedef Vector3<unsigned int> Vector3ui;
typedef Vector3<unsigned int> Point3ui;

 Vector3<float> Vector3f::Up = { 0.0f, 1.0f, 0.0f };
 Vector3<float> Vector3f::Down = { 0.0f, -1.0f, 0.0f };
 Vector3<float> Vector3f::Right = { 1.0f, 0.0f, 0.0f };
 Vector3<float> Vector3f::Left = { -1.0f, 0.0f, 0.0f };
 Vector3<float> Vector3f::Forward = { 0.0f, 0.0f, 1.0f };
 Vector3<float> Vector3f::Back = { 0.0f, 0.0f, -1.0f };
 Vector3<float> Vector3f::Zero = { 0.0f, 0.0f, 0.0f };
 Vector3<float> Vector3f::One = { 1.0f, 1.0f, 1.0f };
 Vector3<float> Vector3f::PosInfinity = { std::numeric_limits<float>::infinity() , std::numeric_limits<float>::infinity() , std::numeric_limits<float>::infinity() };
 Vector3<float> Vector3f::NegInfinity = { -std::numeric_limits<float>::infinity() , -std::numeric_limits<float>::infinity() , -std::numeric_limits<float>::infinity() };



