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

struct Tri{
	uint v1 = 0; uint v2 = 0; uint v3 = 0;
	void set(uint v1, uint v2, uint v3) { this->v1 = v1; this->v2 = v2; this->v3 = v3; }
};

class Mesh {

public:
	Mesh(const aiMesh& mesh);
	Mesh(PrimitiveTypes primitive);
	~Mesh();

	void Draw(Material* mat) const;
	void DrawNormals() const;

	void getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_colors, bool& has_texcoords) const;
	float3 getHalfSize() const { return half_size; }
	float3 getCentroid() const { return centroid; }

private:

	void LoadDataToVRAM();
	void BuildCube(float3& size = (float3)float3::one);
	void BuildPlane(float sx = 1.0f, float sy = 1.0f);
	void BuildSphere(float radius = 1.0f, float sectorCount = 12.0f, float stackCount = 24.0f);
	void BuildCylinder(float radius = 1.0f, float length = 3.0f, int steps = 30);
	bool LoadFromAssimpMesh(const aiMesh& mesh);
	void ClearData();
	void calculateCentroidandHalfsize();

private:

	const uint id = 0;
	uint iboId = 0;
	uint vboId = 0;

	uint num_tris = 0;
	uint num_vertices = 0;
	bool imported_normals = false;
	bool imported_colors = false;
	bool imported_tex_coords = false;

	Tri* tris = nullptr;
	float3* vertices = nullptr;
	float3* normals = nullptr;
	float3* colors = nullptr;
	float2* tex_coords = nullptr;

	float3 half_size = float3::zero;
	float3 centroid = float3::zero;
};
#endif