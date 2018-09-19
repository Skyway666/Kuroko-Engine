#pragma once
#pragma once

#include "MathGeoLib\MathBuildConfig.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\MathGeoLibFwd.h"

#include <limits>

template <class TYPE>
class Vector2
{
public:
	TYPE x = 0.0f;
	TYPE y = 0.0f;

public:

	Vector2() {};
	Vector2(TYPE x, TYPE y) : x(x), y(y) {};
	Vector2(float2& vec)	: x(vec.x), y(vec.y) {};


	float2 toMathVec() { return float2((float)x, (float)y); };

	void setX(TYPE new_x) { x = new_x; };
	void setY(TYPE new_y) { y = new_y; };

	void set(TYPE new_x, TYPE new_y) { x = new_x; y = new_y; };

	Vector2 operator -(const Vector2 &v)   const { return(Vector2(x - v.x, y - v.y)); };
	Vector2 operator +(const Vector2 &v)   const { return(Vector2(x + v.x, y + v.y)); };
	Vector2 operator *(const TYPE scalar) const { return(Vector2(x * scalar, y * scalar)); };

	bool operator ==(const Vector2& v) const { return (x == v.x && y == v.y); };
	bool operator !=(const Vector2& v) const { return (x != v.x || y != v.y); };

	const Vector2& operator -=(const Vector2 &v)
	{
		x -= v.x; y -= v.y;
		return(*this);
	}

	const Vector2& operator +=(const Vector2 &v)
	{
		x += v.x; y += v.y;
		return(*this);
	}

	const Vector2& operator *=(const float &v)
	{
		x *= v; y *= v; 
		return(*this);
	}

	const Vector2& operator /=(const float &v)
	{
		x /= v; y /= v; 
		return(*this);
	}


	// Utils ------------------------------------------------

	bool IsZero() const { return (x == 0 && y == 0); };

	Vector2& SetToZero()
	{
		x = y = 0.0f;
		return(*this);
	}

	Vector2& Negate()
	{
		x = -x; y = -y; 
		return(*this);
	}

	//Module and angle, recommended to use with fpoints or ipoints
	float GetModule() const { return (float)sqrtf((x*x) + (y*y)); };



	void Normalize()
	{
		if (!IsZero())
		{
			float module = GetModule();
			x = (x / module); y = (y / module); 
		}
	}

	Vector2 Normalized()
	{
		Vector2 Normalized = { 0.0f, 0.0f};

		if (!IsZero())
		{
			float module = GetModule();
			Normalized.x = (x / module); Normalized.y = (y / module); 
		}

		return Normalized;
	}

	// Distances ---------------------------------------------

	float DistanceTo(const Vector2& v) const
	{
		float fx = x - v.x;
		float fy = y - v.y;

		return sqrtf((fx*fx) + (fy*fy) );
	}


	static Vector2<float> Up;
	static Vector2<float> Down;
	static Vector2<float> Right;
	static Vector2<float> Left;
	static Vector2<float> Zero;
	static Vector2<float> One;
	static Vector2<float> PosInfinity;
	static Vector2<float> NegInfinity;

};

typedef Vector2<float> Vector2f;
typedef Vector2<float> Point2f;
typedef Vector2<int> Vector2i;
typedef Vector2<int> Point2i;
typedef Vector2<unsigned int> Vector2ui;
typedef Vector2<unsigned int> Point2ui;

Vector2<float> Vector2f::Up = { 0.0f, 1.0f };
Vector2<float> Vector2f::Down = { 0.0f, -1.0f };
Vector2<float> Vector2f::Right = { 1.0f, 0.0f };
Vector2<float> Vector2f::Left = { -1.0f, 0.0f };
Vector2<float> Vector2f::Zero = { 0.0f, 0.0f };
Vector2<float> Vector2f::One = { 1.0f, 1.0f };
Vector2<float> Vector2f::PosInfinity = { std::numeric_limits<float>::infinity() , std::numeric_limits<float>::infinity() };
Vector2<float> Vector2f::NegInfinity = { -std::numeric_limits<float>::infinity() , -std::numeric_limits<float>::infinity()  };

