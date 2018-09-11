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

	mesh_center = ((lowest_p + highest_p) * 0.5f).toMathVec();
	
	obb = new OBB();
	obb->pos = Centroid();
	obb->r = highest_p.toMathVec() - Centroid();

	obb->axis[0] = transform->Right();
	obb->axis[1] = transform->Up();
	obb->axis[2] = transform->Forward();

	aabb = new AABB(obb->MinimalEnclosingAABB());

	last_pos = transform->getPosition();
	last_rot = transform->getRotation();
	last_scl = transform->getScale();;
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
		bool update = false;

		if (transform->getPosition() != last_pos)
		{
			last_pos = transform->getPosition();
			update = true;
		}

		if (!transform->getRotation().Equals(last_rot))
		{
			obb->axis[0] = transform->Right();
			obb->axis[1] = transform->Up();
			obb->axis[2] = transform->Forward();

			last_rot = transform->getRotation();
			update = true;
		}

		if (transform->getScale() != last_scl)
		{
			Vector3f scale = transform->getScale();
			Vector3f scale_proportion = { scale.x / last_scl.x, scale.y / last_scl.y, scale.z / last_scl.z };
			obb->r = { obb->r.x * scale_proportion.x, obb->r.y * scale_proportion.y, obb->r.z * scale_proportion.z };

			last_scl = scale;
			update = true;
		}

		if (update)
		{
			obb->pos = Centroid();
			*aabb = obb->MinimalEnclosingAABB();
		}

		if (draw_aabb)
			DrawAABB();
		if (draw_obb)
			DrawOBB();
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

void ComponentAABB::DrawOBB()
{
	glLineWidth(1.5f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);

	for (int i = 0; i < 12; i++)
		{ glVertex3f(obb->Edge(i).a.x, obb->Edge(i).a.y, obb->Edge(i).a.z);		glVertex3f(obb->Edge(i).b.x, obb->Edge(i).b.y, obb->Edge(i).b.z); }
	
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

float3 ComponentAABB::Centroid()
{
	float3 centroid = { mesh_center.x * transform->getScale().x, mesh_center.y * transform->getScale().y , mesh_center.z * transform->getScale().z };
	centroid = transform->getRotation() * centroid;
	centroid += transform->getPosition().toMathVec();
	return centroid;
}

Vector3f ComponentAABB::getCentroid()
{
	float3 centroid = Centroid();
	return Vector3f(centroid.x, centroid.y, centroid.z);
}