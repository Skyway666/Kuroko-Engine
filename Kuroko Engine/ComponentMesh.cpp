#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Material.h"
#include "ModuleImporter.h"
#include "FileSystem.h"
#include "glew-2.1.0\include\GL\glew.h"

ComponentMesh::ComponentMesh(JSON_Object * deff): Component(nullptr, MESH) {
	std::string path;

	// Load mesh from own file format
	PrimitiveTypes primitive_type = Primitive_None;
	std::string mesh_name = json_object_get_string(deff, "mesh_name");

	if (whichPrimitive(mesh_name, primitive_type)) { // If it is a primitive, build one, else load from .kr
		mesh = new Mesh(primitive_type);
	}
	else{
	App->fs->FormFullPath(path, mesh_name.c_str(), LIBRARY_MESHES, ENGINE_EXTENSION);
	mesh = App->importer->ImportMeshFromKR(path.c_str());
	}

	mesh->setName(mesh_name.c_str());
	mat = new Material();
	const char* diffuse_name;
	if(diffuse_name = json_object_get_string(deff, "diffuse_name")){ // If it has a diffuse texture load it
		App->fs->FormFullPath(path, diffuse_name, LIBRARY_TEXTURES, DDS_EXTENSION);
		Texture* diffuse = (Texture*)App->importer->Import(path.c_str(), I_TEXTURE);
		mat->setTexture(DIFFUSE, diffuse);
	}
	
}

void ComponentMesh::Draw() const
{
	ComponentTransform* transform = nullptr;
	float4x4 view_mat = float4x4::identity;

	if (transform = (ComponentTransform*)getParent()->getComponent(TRANSFORM))
	{
		GLfloat matrix[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
		view_mat.Set((float*)matrix);

		glMatrixMode(GL_MODELVIEW_MATRIX);
		glLoadMatrixf((GLfloat*)(transform->global->getMatrix().Transposed() * view_mat).v);
	}

	if (draw_normals || App->scene->global_normals)
		mesh->DrawNormals();

	if (wireframe || App->scene->global_wireframe)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else											glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mesh->Draw(mat);

	if (transform)
		glLoadMatrixf((GLfloat*)view_mat.v);
}

bool ComponentMesh::whichPrimitive(std::string mesh_name, PrimitiveTypes & which_primitive) {

	which_primitive = Primitive_None; // Just for security
	if (mesh_name.compare("CUBE") == 0) 
		which_primitive = Primitive_Cube;

	else if (mesh_name.compare("PLANE") == 0) 
		which_primitive = Primitive_Plane;

	else if (mesh_name.compare("SPHERE") == 0) 
		which_primitive = Primitive_Sphere;

	else if (mesh_name.compare("CYLINDER") == 0) 
		which_primitive = Primitive_Cylinder;
	
	if (which_primitive != Primitive_None)
		return true;

	return false;
}

void ComponentMesh::Save(JSON_Object & config) {
	// Determine the type of the mesh
 	// Component has two strings, one for mesh name, and another for diffuse texture name
	json_object_set_string(&config, "type", "mesh");
	json_object_set_string(&config, "mesh_name", mesh->getName());
	if(mat && mat->getTexture(DIFFUSE))  //If it has a material and a diffuse texture
		json_object_set_string(&config, "diffuse_name", mat->getTexture(DIFFUSE)->getName());
}

void ComponentMesh::Load(JSON_Object & config) {

}
