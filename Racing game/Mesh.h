#pragma once
#include "Globals.h"
#include "Math.h"

class Mesh{
public:

	Mesh();
	void LoadDataToVRAM();

	void Draw();


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
