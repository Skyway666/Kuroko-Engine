#ifndef _COMPONENT_MESH
#define _COMPONENT_MESH
#include "Component.h"
#include "Mesh.h"

class Material;

class ComponentMesh : public Component {

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};   // empty constructor
	ComponentMesh(JSON_Object* deff, GameObject* parent);
	ComponentMesh(GameObject* gameobject, Mesh* mesh) : Component(gameobject, MESH), primitive_mesh(mesh) {};

	void Draw() const;
	void DrawSelected() const;
	bool getWireframe() const			{ return wireframe; };
	bool getDrawNormals()const			{ return draw_normals; };
	Material* getMaterial()	const		{ return mat; };
	void setWireframe(bool state)		{ wireframe = state; };
	void setDrawNormals(bool state)		{ draw_normals = state; };


	Mesh* getMesh()	const;
	void setMaterial(Material* new_mat) { mat = new_mat; };// Can recieve nullptr
	void setMesh(Mesh* new_mesh)		{ primitive_mesh = new_mesh; }// Can recieve nullptr
	bool whichPrimitive(std::string mesh_name, PrimitiveTypes& which_primitive); 

	void Save(JSON_Object* config);

private:
	bool wireframe = false;
	bool draw_normals = false;

	Material* mat = nullptr;
	Mesh* primitive_mesh = nullptr;

	uint mesh_resource_uuid;
};
#endif