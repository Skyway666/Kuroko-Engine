#pragma once
#include "Globals.h"
#include "Vector3.h"
#include "Primitive.h"

class Mesh{
public:

	Mesh(PrimitiveTypes primitive);
	~Mesh();
	void LoadDataToVRAM();

	void Draw();
private:

	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);

private:
	uint id_tris = 0;
	uint num_tris = 0;
	Point3i* tris = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	Point3f* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	Point3f* normals = nullptr;

};
