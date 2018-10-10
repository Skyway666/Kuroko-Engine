#ifndef _MESH
#define _MESH
#include "Globals.h"

#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\float2.h"

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

	void Draw(Material* mat) const;
	void DrawNormals() const;

	void getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_colors, bool& has_texcoords) const;
	float3 getHalfSize() const { return half_size; }
	float3 getCentroid() const { return centroid; }

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

	float3* tris = nullptr;
	float3* vertices = nullptr;
	float3* normals = nullptr;
	float3* colors = nullptr;
	float2* tex_coords = nullptr;

	float3 half_size = float3::zero;
	float3 centroid = float3::zero;
};
#endif