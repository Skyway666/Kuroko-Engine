#include "ComponentAABB.h"
#include "ComponentMesh.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

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

	float3 half_size, centroid;
	getParent()->getInheritedHalfsizeAndCentroid(half_size, centroid);
	
	obb->pos = centroid;
	obb->r = half_size;

	transform = (ComponentTransform*)getParent()->getComponent(TRANSFORM);

	obb->axis[0] = transform->Right();
	obb->axis[1] = transform->Up();
	obb->axis[2] = transform->Forward();

	if (!aabb)
		aabb = new AABB();

	*aabb = obb->MinimalEnclosingAABB();
}

bool ComponentAABB::Update(float dt)
{
	if (isActive())
	{
		bool update = false;

		float3 inh_half_size, inh_centroid;
		getParent()->getInheritedHalfsizeAndCentroid(inh_half_size, inh_centroid);

		if (!inh_centroid.Equals(obb->pos))
		{
			obb->pos = inh_centroid;
			update = true;
		}

		if (!transform->Right().Equals(obb->axis[0]))
		{
			obb->axis[0] = transform->Right();
			obb->axis[1] = transform->Up();
			obb->axis[2] = transform->Forward();

			update = true;
		}


		if (!inh_half_size.Equals(obb->r))
		{
			obb->r = inh_half_size;
			update = true;
		}

		if (update)
			*aabb = obb->MinimalEnclosingAABB();
		
		
		if (draw_aabb)
			DrawAABB();
		if (draw_obb)
			DrawOBB();
	}
	return true;
}

void ComponentAABB::DrawAABB() const
{
	App->renderer3D->DrawDirectAABB(*aabb);
}

void ComponentAABB::DrawOBB() const
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

