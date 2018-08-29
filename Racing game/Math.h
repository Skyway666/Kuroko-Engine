#pragma once

#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\MathGeoLibFwd.h"
#include "p2Point.h"
#include "Globals.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

using namespace math;

class RNG
{
private:
	LCG* lcg = nullptr;

public:

	RNG() { lcg = new LCG(); };
	~RNG() { delete lcg; }

	float Zero_to_One()					{ return lcg->Float(); }			// returns a random float between 0 and 1
	float Range(float min, float max)	{ return lcg->Float(min, max); }	// returns a random float between min and max
	float RangeInt(int min, int max)	{ return lcg->Int(min, max); }		// returns a random int between min and max

};



class Vector3
{
private:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

public:

	Vector3() {};
	Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

	float3	  toMathVec()   { return float3(x, y, z); };
	btVector3 toBtVec()		{ return btVector3(x, y, z); };
	vec3	  toGlVec()		{ return vec3(x, y, z); };

	void setX(float new_x) { x = new_x; };
	void setY(float new_y) { y = new_y; };
	void setZ(float new_z) { z = new_z; };

	void set(float new_x, float new_y, float new_z)		{ x = new_x; y = new_y; z = new_z; };

	Vector3 operator -(const Vector3 &v)   const { return(Vector3(x - v.x, y - v.y, z - v.z)); };
	Vector3 operator +(const Vector3 &v)   const { return(Vector3(x + v.x, y + v.y, z + v.z)); };
	Vector3 operator *(const float scalar) const { return(Vector3(x * scalar, y * scalar, z * scalar)); };

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


	// Utils ------------------------------------------------

	bool IsZero() const { return (x == 0 && y == 0 && z == 0); };

	Vector3& SetToZero()
	{
		x = y = z = 0;
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

};

typedef Vector3 Point3;





