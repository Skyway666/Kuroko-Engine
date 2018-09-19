
#pragma once
#include "Color.h"
#include "Vector3.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color ambient;
	Color diffuse;
	Vector3f position;

	int ref;
	bool on;
};