#include "ComponentAABB.h"
#include "ComponentMesh.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "ComponentTransform.h"

ComponentAABB::ComponentAABB(GameObject* parent) : Component(parent, C_AABB)
{
	std::list<Component*> meshes;
	getAllMeshes(getParent(), meshes);

	Vector3f lowest_p = { 0.0f, 0.0f, 0.0f };
	Vector3f highest_p = { 0.0f, 0.0f, 0.0f };

	if (!meshes.empty())
		lowest_p = highest_p = ((ComponentMesh*)meshes.front())->vertices[0];

	for (std::list<Component*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		ComponentMesh* mesh = (ComponentMesh*)(*it);

		for (int i = 0; i < mesh->num_vertices; i++)
		{
			if (lowest_p.x > mesh->vertices[i].x) lowest_p.x = mesh->vertices[i].x;
			if (lowest_p.y > mesh->vertices[i].y) lowest_p.y = mesh->vertices[i].y;
			if (lowest_p.z > mesh->vertices[i].z) lowest_p.z = mesh->vertices[i].z;

			if (highest_p.x < mesh->vertices[i].x) highest_p.x = mesh->vertices[i].x;
			if (highest_p.y < mesh->vertices[i].y) highest_p.y = mesh->vertices[i].y;
			if (highest_p.z < mesh->vertices[i].z) highest_p.z = mesh->vertices[i].z;
		}
	}

	transform = (ComponentTransform*)getParent()->getComponent(TRANSFORM);

	lowest_p += transform->position; highest_p += transform->position;
	last_pos = transform->position;

	lowest_p.x *= transform->scale.x; lowest_p.y *= transform->scale.y; lowest_p.z *= transform->scale.z;
	highest_p.x *= transform->scale.x; highest_p.y *= transform->scale.y; highest_p.z *= transform->scale.z;
	last_scl = transform->scale;
	
	aabb = new AABB(lowest_p.toMathVec(), highest_p.toMathVec());
	obb = new OBB(aabb->ToOBB());

	Vector3f euler_rot = transform->getRotationEuler();
	float3x3 rotation_mat = float3x3::identity;
	rotation_mat.RotateX(euler_rot.x); rotation_mat.RotateY(euler_rot.y); rotation_mat.RotateZ(euler_rot.z);
	obb->Transform(rotation_mat);
	last_rot = euler_rot;

	*aabb = obb->MinimalEnclosingAABB();
}

ComponentAABB::~ComponentAABB()
{
	if (aabb) delete aabb;
	if (obb) delete obb;
}

bool ComponentAABB::Update(float dt)
{
	if (isActive())
	{
		if (transform->position != last_pos)
		{
			aabb->Translate((transform->position - last_pos).toMathVec()); obb->Translate((transform->position - last_pos).toMathVec());
			last_pos = transform->position;
		}

		if ((transform->getRotationEuler()) != last_rot)
		{
			Vector3f euler_rot = transform->getRotationEuler();
			float3x3 rotation_mat = float3x3::FromEulerXYZ(euler_rot.x- last_rot.x, euler_rot.y - last_rot.y, euler_rot.z - last_rot.z);
			obb->Transform(rotation_mat);
			last_rot = euler_rot;

			*aabb = obb->MinimalEnclosingAABB();
		}

		if (transform->scale != last_scl)
		{
			Vector3f scl_propotion = { transform->scale.x / last_scl.x, transform->scale.y / last_scl.y, transform->scale.z / last_scl.z };
			aabb->Scale(aabb->CenterPoint(), scl_propotion.toMathVec());
			obb->Scale(aabb->CenterPoint(), scl_propotion.toMathVec());
			last_scl = transform->scale;
		}

		if (draw)
			DrawAABB();
	}
	return true;
}

void ComponentAABB::DrawAABB()
{
	Vector3f lowest_p = { aabb->minPoint.x, aabb->minPoint.y,aabb->minPoint.z };
	Vector3f highest_p = { aabb->maxPoint.x, aabb->maxPoint.y,aabb->maxPoint.z };

	glLineWidth(1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINES);

	glVertex3f(lowest_p.x, lowest_p.y, lowest_p.z);		glVertex3f(highest_p.x, lowest_p.y, lowest_p.z);
	glVertex3f(lowest_p.x, lowest_p.y, lowest_p.z);		glVertex3f(lowest_p.x, highest_p.y, lowest_p.z);
	glVertex3f(highest_p.x, lowest_p.y, lowest_p.z);	glVertex3f(highest_p.x, highest_p.y, lowest_p.z);
	glVertex3f(lowest_p.x, highest_p.y, lowest_p.z);	glVertex3f(highest_p.x, highest_p.y, lowest_p.z);

	glVertex3f(highest_p.x, highest_p.y, highest_p.z);  glVertex3f(lowest_p.x, highest_p.y, highest_p.z);
	glVertex3f(highest_p.x, highest_p.y, highest_p.z);  glVertex3f(highest_p.x, lowest_p.y, highest_p.z);
	glVertex3f(lowest_p.x, highest_p.y, highest_p.z);	glVertex3f(lowest_p.x, lowest_p.y, highest_p.z);
	glVertex3f(highest_p.x, lowest_p.y, highest_p.z);	glVertex3f(lowest_p.x, lowest_p.y, highest_p.z);

	glVertex3f(lowest_p.x, lowest_p.y, lowest_p.z);		glVertex3f(lowest_p.x, lowest_p.y, highest_p.z);
	glVertex3f(highest_p.x, highest_p.y, lowest_p.z);	glVertex3f(highest_p.x, highest_p.y, highest_p.z);
	glVertex3f(highest_p.x, lowest_p.y, lowest_p.z);	glVertex3f(highest_p.x, lowest_p.y, highest_p.z);
	glVertex3f(lowest_p.x, highest_p.y, lowest_p.z);	glVertex3f(lowest_p.x, highest_p.y, highest_p.z);

	glEnd();
	glColor3f(1.0f, 1.0f, 1.0f);

	glLineWidth(1.0f);
}


void ComponentAABB::getAllMeshes(GameObject* obj,std::list<Component*>& list_to_fill)
{
	obj->getComponents(MESH, list_to_fill);

	std::list<GameObject*> children;
	obj->getChildren(children);

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
		getAllMeshes(*it, list_to_fill);
}