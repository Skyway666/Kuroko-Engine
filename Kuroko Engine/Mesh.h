#pragma once
#include "Globals.h"
#include "Vector3.h"
#include "Vector2.h"

class aiMesh;
class Material;

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Mesh {

public:

	Mesh(aiMesh* mesh);
	Mesh(PrimitiveTypes primitive);
	~Mesh();

	void Draw(Material* mat);
	void DrawNormals();
	void getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_colors, bool& has_texcoords) const;
	Vector3f getHalfSize() const { return half_size; }
	Vector3f getCentroid() const { return centroid; }

private:

	void LoadDataToVRAM();
	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	void BuildPlane(float sx = 1.0f, float sy = 1.0f);
	void BuildSphere(float radius = 1.0f, float sectorCount = 12.0f, float stackCount = 24.0f);
	void BuildCylinder(float radius = 1.0f, float length = 3.0f, int steps = 30);
	bool LoadFromAssimpMesh(aiMesh* mesh);
	void ClearData();
	void calculateCentroidandHalfsize();

private:

	uint iboId = 0;
	uint vboId = 0;

	uint num_tris = 0;
	uint num_vertices = 0;
	bool imported_normals = false;
	bool imported_colors = false;
	bool imported_tex_coords = false;

	Point3ui* tris = nullptr;
	Point3f* vertices = nullptr;
	Point3f* normals = nullptr;
	Point3f* colors = nullptr;
	Point2f* tex_coords = nullptr;

	Vector3f half_size = Vector3f::Zero;
	Vector3f centroid = Vector3f::Zero;
};
