#ifndef _COMPONENT_MESH
#define _COMPONENT_MESH
#include "Component.h"
#include "Mesh.h"

class Material;

class ComponentMesh : public Component {

public:

	ComponentMesh(GameObject* gameobject) : Component(gameobject, MESH) {};   // empty constructor
	ComponentMesh(JSON_Object* deff, GameObject* parent);
	//ComponentMesh(GameObject* gameobject, Mesh* mesh) : Component(gameobject, MESH), primitive_mesh(mesh) {};
	ComponentMesh(GameObject* gameobject, PrimitiveTypes type);

	void Draw() const;
	void DrawSelected() const;
	bool getWireframe() const			{ return wireframe; };
	bool getDrawNormals()const			{ return draw_normals; };
	Material* getMaterial()	const		{ return mat; };
	void setWireframe(bool state)		{ wireframe = state; };
	void setDrawNormals(bool state)		{ draw_normals = state; };


	Mesh* getMesh()	const;
	void setMaterial(Material* new_mat) { mat = new_mat; };// Can recieve nullptr
	void setMeshResourceId(uint _mesh_resource_uuid) { mesh_resource_uuid = _mesh_resource_uuid; }
	PrimitiveTypes primitiveString2PrimitiveType(std::string primitive_type_string);
	std::string PrimitiveType2primitiveString(PrimitiveTypes type);

	void Save(JSON_Object* config);

private:
	bool wireframe = false;
	bool draw_normals = false;

	Material* mat = nullptr;

	uint mesh_resource_uuid = -1;
	PrimitiveTypes primitive_type = PrimitiveTypes::Primitive_None;
};
#endif