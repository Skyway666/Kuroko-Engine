#pragma once
#include "Globals.h"
#include "Vector3.h"
#include "Primitive.h"
#include "Component.h"

class aiMesh;

class ComponentMesh : public Component{

	friend class ModuleFBXimporter;

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};
	ComponentMesh(GameObject* gameobject, aiMesh* mesh);
	ComponentMesh(GameObject* gameobject, PrimitiveTypes primitive);
	~ComponentMesh();

	void LoadDataToVRAM();

	bool Update(float dt) { if (loaded && is_active) Draw(); return true; };
	void Draw();

	void setWireframe(bool state) { wireframe = state; };

	void getData(uint& vert_num, uint& poly_count, bool& has_normals);

private:

	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	bool LoadFromAssimpMesh(aiMesh* mesh);

private:
	bool loaded = false;
	bool wireframe = false;

	uint id_tris = 0;
	uint num_tris = 0;
	Point3ui* tris = nullptr;

	uint id_vertices = 0;
	uint num_vertices = 0;
	Point3f* vertices = nullptr;

	uint id_normals = 0;
	uint num_normals = 0;
	Point3f* normals = nullptr;

};
