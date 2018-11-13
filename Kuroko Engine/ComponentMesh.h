#ifndef _COMPONENT_MESH
#define _COMPONENT_MESH
#include "Component.h"
#include "Mesh.h"

class Material;

class ComponentMesh : public Component {

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};   // empty constructor
	ComponentMesh(JSON_Object* deff, GameObject* parent);
	ComponentMesh(GameObject* gameobject, Mesh* mesh) : Component(gameobject, MESH), mesh(mesh) {};

	void Draw() const;
	void DrawSelected() const;

	bool getWireframe() const			{ return wireframe; };
	bool getDrawNormals()const			{ return draw_normals; };
	Material* getMaterial()	const		{ return mat; };
	Mesh* getMesh()	const				{ return mesh; }
	void setWireframe(bool state)		{ wireframe = state; };
	void setDrawNormals(bool state)		{ draw_normals = state; };
	void setMaterial(Material* new_mat) { mat = new_mat; };// Can recieve nullptr
	void setMesh(Mesh* new_mesh)		{ mesh = new_mesh; }// Can recieve nullptr
	bool whichPrimitive(std::string mesh_name, PrimitiveTypes& which_primitive); 

	void Save(JSON_Object* config);

private:
	bool wireframe = false;
	bool draw_normals = false;

	Material* mat = nullptr;
	Mesh* mesh = nullptr;

	uint mesh_resource;
};
#endif