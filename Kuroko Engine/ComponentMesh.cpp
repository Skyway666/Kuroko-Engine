#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "ComponentAABB.h"
#include "Material.h"
#include "ModuleImporter.h"
#include "FileSystem.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "ModuleCamera3D.h"
#include "Camera.h"
#include "Applog.h"
#include "ModuleResourcesManager.h"
#include "ResourceMesh.h"

ComponentMesh::ComponentMesh(JSON_Object * deff, GameObject* parent): Component(parent, MESH) {
	std::string path;

	// Load mesh from own file format
	PrimitiveTypes primitive_type = Primitive_None;
	std::string mesh_name = json_object_get_string(deff, "mesh_name"); 

	if (whichPrimitive(mesh_name, primitive_type)) { // If it is a primitive, build one, else load from .kr
		primitive_mesh = new Mesh(primitive_type);			 // TODO: Store the color of the meshes
		primitive_mesh->setName(mesh_name.c_str());
	}
	else{
		// ASSIGNING RESOURCE
		mesh_resource_uuid = json_object_get_number(deff, "mesh_resource_uuid");
		App->resources->assignResource(mesh_resource_uuid);
	}


	mat = new Material();
	// ASSIGNING RESOURCE
	uint diffuse_resource = json_object_dotget_number(deff, "material.diffuse_resource_uuid");
	App->resources->assignResource(diffuse_resource);
	mat->setTextureResource(DIFFUSE, diffuse_resource);
}

void ComponentMesh::Draw() const
{
	OBB* obb = ((ComponentAABB*)getParent()->getComponent(C_AABB))->getOBB();

	if (App->camera->current_camera->frustumCull(*obb))
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

		ResourceMesh* mesh_resource = (ResourceMesh*)App->resources->getResource(mesh_resource_uuid);
		Mesh* mesh_from_resource = mesh_resource->mesh;
		

		if (draw_normals || App->scene->global_normals)
			mesh_from_resource->DrawNormals();

		if (wireframe || App->scene->global_wireframe)	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else											glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		mesh_from_resource->Draw(mat);

		if (transform)
			glLoadMatrixf((GLfloat*)view_mat.v);
	}
}

void ComponentMesh::DrawSelected() const
{

	OBB* obb = ((ComponentAABB*)getParent()->getComponent(C_AABB))->getOBB();

	if (App->camera->current_camera->frustumCull(*obb))
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

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		ResourceMesh* mesh_resource = (ResourceMesh*)App->resources->getResource(mesh_resource_uuid);
		Mesh* mesh_from_resource = mesh_resource->mesh;
		mesh_from_resource->Draw(nullptr, true);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (transform)
			glLoadMatrixf((GLfloat*)view_mat.v);
	}
}

Mesh* ComponentMesh::getMesh() const {

	Mesh* ret = nullptr;
	if (primitive_mesh) {
		ret = primitive_mesh;
	}
	else {
		if(ResourceMesh* resource_mesh = (ResourceMesh*)App->resources->getResource(mesh_resource_uuid))
		ret = resource_mesh->mesh;
	}
	return ret;
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

void ComponentMesh::Save(JSON_Object* config) {
	// Determine the type of the mesh
 	// Component has two strings, one for mesh name, and another for diffuse texture name
	json_object_set_string(config, "type", "mesh");
	json_object_set_number(config, "mesh_resource_uuid", mesh_resource_uuid);
	if(mat)  //If it has a material and a diffuse texture
		json_object_dotset_number(config, "material.diffuse_resource_uuid", mat->getTextureResource(DIFFUSE));
}
