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
	
	//std::string mesh_name = json_object_get_string(deff, "mesh_name"); // Mesh name not used for now

	primitive_type = primitiveString2PrimitiveType(json_object_get_string(deff, "primitive_type"));


															 
	if(primitive_type == Primitive_None){			// TODO: Store the color of the meshes
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

ComponentMesh::ComponentMesh(GameObject* gameobject, PrimitiveTypes type) : Component(gameobject, MESH) {
	primitive_type = type;
	mat = new Material();

}
ComponentMesh::~ComponentMesh() {
	// Deassign all the components that the element had if it is deleted
	App->resources->deasignResource(mesh_resource_uuid);
	delete mat;
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

		Mesh* mesh_from_resource = getMeshFromResource();
		
		if (!mesh_from_resource)
			return;

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
		

		Mesh* mesh_from_resource = getMeshFromResource();

		if (!mesh_from_resource)
			return;

		mesh_from_resource->Draw(nullptr, true);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		if (transform)
			glLoadMatrixf((GLfloat*)view_mat.v);
	}
}

Mesh* ComponentMesh::getMesh() const {

	Mesh* ret = nullptr;
	ResourceMesh* mesh_resource = nullptr;
	if (primitive_type == Primitive_None) {
		mesh_resource = (ResourceMesh*)App->resources->getResource(mesh_resource_uuid);
		if(mesh_resource)
			ret = mesh_resource->mesh;
	}
	else {
		mesh_resource = (ResourceMesh*)App->resources->getPrimitiveMeshResource(primitive_type);
		ret = mesh_resource->mesh;
	}
	return ret;
}
PrimitiveTypes ComponentMesh::primitiveString2PrimitiveType(std::string primitive_type_string) {

	PrimitiveTypes ret = Primitive_None; // Just for security
	if (primitive_type_string == "CUBE")
		ret = Primitive_Cube;
	else if (primitive_type_string == "PLANE")
		ret = Primitive_Plane;
	else if (primitive_type_string == "SPHERE")
		ret = Primitive_Sphere;
	else if (primitive_type_string == "CYLINDER")
		ret = Primitive_Cylinder;
	
	return ret;
}

std::string ComponentMesh::PrimitiveType2primitiveString(PrimitiveTypes type) {
	std::string ret = "NONE";

	switch (type) {
	case Primitive_Cube:
		ret = "CUBE";
		break;
	case Primitive_Plane:
		ret = "PLANE";
		break;
	case Primitive_Sphere:
		ret = "SPHERE";
		break;
	case Primitive_Cylinder:
		ret = "CYLINDER";
		break;

	}
	return ret;
}

void ComponentMesh::Save(JSON_Object* config) {
	// Determine the type of the mesh
 	// Component has two strings, one for mesh name, and another for diffuse texture name
	json_object_set_string(config, "type", "mesh");
	json_object_set_number(config, "mesh_resource_uuid", mesh_resource_uuid);
	json_object_set_string(config, "primitive_type", PrimitiveType2primitiveString(primitive_type).c_str());
	if(mat)  //If it has a material and a diffuse texture
		json_object_dotset_number(config, "material.diffuse_resource_uuid", mat->getTextureResource(DIFFUSE));
}

Mesh * ComponentMesh::getMeshFromResource() const
{
	ResourceMesh* mesh_resource = nullptr;
	Mesh* mesh = nullptr;

	if (primitive_type == Primitive_None) {
		mesh_resource = (ResourceMesh*)App->resources->getResource(mesh_resource_uuid);
		if (mesh_resource)
			mesh = mesh_resource->mesh;
	}
	else {
		mesh_resource = (ResourceMesh*)App->resources->getPrimitiveMeshResource(primitive_type);
		mesh = mesh_resource->mesh;
	}
	return mesh;
}
