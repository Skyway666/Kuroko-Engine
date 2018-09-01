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

	bool Update() { if (loaded) Draw(); };
	void Draw();
private:

	void BuildCube(float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);
	bool LoadFromAssimpMesh(aiMesh* mesh);

private:
	bool loaded = false;

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
