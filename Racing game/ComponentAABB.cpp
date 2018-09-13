#include "ComponentAABB.h"
#include "ComponentMesh.h"
#include "GameObject.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "ComponentTransform.h"


ComponentAABB::~ComponentAABB()
{
	if (aabb) delete aabb;
	if (obb) delete obb;
}

void ComponentAABB::Reload()
{
	if (!obb)
		obb = new OBB();

	obb->pos = getParent()->getInheritedCenter().toMathVec();
	obb->r = getParent()->getInheritedHalfsize().toMathVec();

	transform = (ComponentTransform*)getParent()->getComponent(TRANSFORM);

	obb->axis[0] = transform->Right();
	obb->axis[1] = transform->Up();
	obb->axis[2] = transform->Forward();

	if (!aabb)
		aabb = new AABB();

	*aabb = obb->MinimalEnclosingAABB();

	last_pos = getParent()->getInheritedCenter();
	last_rot = transform->getRotation();
	last_scl = transform->getScale();


	Vector3f scale_proportion = { last_scl.x / 1.0f, last_scl.y / 1.0f, last_scl.z / 1.0f };
	obb->r = { obb->r.x * scale_proportion.x, obb->r.y * scale_proportion.y, obb->r.z * scale_proportion.z };
}

bool ComponentAABB::Update(float dt)
{
	if (isActive())
	{
		bool update = false;

		if (getParent()->getInheritedCenter() != last_pos)
		{
			last_pos = getParent()->getInheritedCenter();
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
			obb->pos = getParent()->getInheritedCenter().toMathVec();
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

