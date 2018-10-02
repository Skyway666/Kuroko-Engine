#pragma once
#include "Globals.h"
#include "Component.h"
#include "Material.h"

#include "Vector3.h"
#include "Vector2.h"

class aiMesh;

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class ComponentMesh : public Component{

	friend class ModuleImporter;
	friend class ComponentAABB;
	friend class GameObject;

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};
	ComponentMesh(GameObject* gameobject, aiMesh* mesh);
	ComponentMesh(GameObject* gameobject, PrimitiveTypes primitive);
	~ComponentMesh();

	bool Update(float dt) { if (loaded && isActive()) Draw(); return true; };
	void Draw();

	bool getWireframe() { return wireframe; };
	void setWireframe(bool state) { wireframe = state; };
	Material* getMaterial() { return mat; };
	void setMaterial(Material* new_mat) { mat = new_mat; };

	void getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_colors, bool& has_texcoords);
	void assignCheckeredMat(TextureType type = DIFFUSE);

private:

	void LoadDataToVRAM();
	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	void BuildPlane(float sx = 1.0f, float sy = 1.0f);
	bool LoadFromAssimpMesh(aiMesh* mesh);
	void ClearData();

private:
	void calculateHalfsize();

private:
	bool loaded = false;
	bool wireframe = false;

	Material* mat = nullptr;

	//Mesh data
	uint iboId = 0;
	uint vboId = 0;

	uint num_tris = 0;
	uint num_vertices = 0;
	bool imported_normals = false;
	bool imported_colors = false;
	bool imported_tex_coords = false;

	Point3ui* tris		= nullptr;
	Point3f* vertices	= nullptr;
	Point3f* normals	= nullptr;
	Point3f* colors		= nullptr;
	Point2f* tex_coords	= nullptr;

	Vector3f half_size = Vector3f::Zero;
};
