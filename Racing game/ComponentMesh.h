#pragma once
#include "Globals.h"
#include "Primitive.h"
#include "Component.h"

#include "Vector3.h"
#include "p2Point.h"

class aiMesh;
class Material;

class ComponentMesh : public Component{

	friend class ModuleFBXimporter;

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};
	ComponentMesh(GameObject* gameobject, aiMesh* mesh);
	ComponentMesh(GameObject* gameobject, PrimitiveTypes primitive);
	~ComponentMesh();

	bool Update(float dt) { if (loaded && is_active) Draw(); return true; };
	void Draw();

	void setWireframe(bool state) { wireframe = state; };

	void getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_texcoords);
	void assignCheckeredMat();

private:

	void LoadDataToVRAM();
	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	bool LoadFromAssimpMesh(aiMesh* mesh);

private:
	bool loaded = false;
	bool wireframe = false;

	Material* mat = nullptr;

	//Mesh data
	uint id_tris = 0;
	uint num_tris = 0;
	Point3ui* tris = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	Point3f* vertices = nullptr;

	uint id_normals = 0;
	Point3f* normals = nullptr;

	uint id_tex_coords = 0;
	fPoint* tex_coords = nullptr;

};
