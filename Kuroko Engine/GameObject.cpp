#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentAABB.h"
#include "Application.h"
#include "ModuleImGUI.h"
#include "ModuleSceneIntro.h"
#include "Applog.h"


GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	addComponent(TRANSFORM);
	id = ++App->scene_intro->last_gobj_id;

	//Just for assignment 1
	App->scene_intro->selected_obj = this;
}

GameObject::~GameObject()
{
	for (auto it = children.begin(); it != children.end(); it++)
		delete *it;

	for (auto it = components.begin(); it != components.end(); it++)
		delete *it;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	if (is_active)
	{
		for (std::list<Component*>::iterator it = components.begin(); it != components.end() && ret; it++)
			ret = (*it)->Update(dt);

		for (std::list<GameObject*>::iterator it = children.begin(); it != children.end() && ret; it++)
			ret = (*it)->Update(dt);

		if (!ret)
			app_log->AddLog("error in gameobject %s", name.c_str());
	}

	return ret;
}


Component* GameObject::getComponent(Component_type type)
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++)
	{
		if ((*it)->getType() == type)
			return *it;
	}

	return nullptr;
}

bool GameObject::getComponents(Component_type type, std::list<Component*>& list_to_fill)
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++)
	{
		if ((*it)->getType() == type)
			list_to_fill.push_back(*it);
	}

	return !list_to_fill.empty();
}

GameObject* GameObject::getChild(const char* name) 
{
	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->getName() == name)
			return *it;
	}

	return nullptr;
}

Component* GameObject::addComponent(Component_type type)
{
	Component* new_component = nullptr;

	switch (type)
	{
	case MESH:		
		new_component = new ComponentMesh(this); 
		components.push_back(new_component);
		calculateCentroidandHalfsize();
		if (ComponentAABB* aabb = (ComponentAABB*)getComponent(C_AABB))
			aabb->Reload();
		break;
	case C_AABB:	
		if (!getComponent(C_AABB))
		{
			new_component = new ComponentAABB(this);
			components.push_back(new_component);
		}
	case TRANSFORM:
		if (!getComponent(TRANSFORM))
		{
			new_component = new ComponentTransform(this);
			components.push_back(new_component);
		}
	default:
		break;
	}

	return new_component;
}

void GameObject::addComponent(Component* component)
{
	if (!component)
		return;
	switch (component->getType())
	{
	case MESH:	
		components.push_back(component);
		calculateCentroidandHalfsize();
		if(Mesh* mesh = ((ComponentMesh*)component)->getMesh())
			((ComponentTransform*)getComponent(TRANSFORM))->setPosition((mesh->getCentroid()));
		if (ComponentAABB* aabb = (ComponentAABB*)getComponent(C_AABB))
			aabb->Reload();
		break;
	case C_AABB:
		if (!getComponent(C_AABB))
			components.push_back(component);
		break;
	case TRANSFORM:
		if (!getComponent(TRANSFORM))
			components.push_back(component);
		break;
	default:
		break;
	}
}


void GameObject::removeComponent(Component* component)
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++)
	{
		if (component->getType() == TRANSFORM)
			continue;
		if (*it == component)
		{
			bool mesh = (component->getType() == MESH);
			components.remove(component);
			if (getComponent(C_AABB) && mesh)
			{
				calculateCentroidandHalfsize();
				ComponentAABB* aabb = (ComponentAABB*)getComponent(C_AABB);
				aabb->Reload();
			}
			break;
		}
	}
	
}


void GameObject::calculateCentroidandHalfsize()
{
	std::list<Component*> meshes;
	getComponents(MESH, meshes);

	float3 lowest_p = float3::inf;
	float3 highest_p = -float3::inf;

	for (std::list<Component*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if (Mesh* mesh = ((ComponentMesh*)(*it))->getMesh())
		{
			float3 half_size = mesh->getHalfSize();

			if (lowest_p.x > -half_size.x) lowest_p.x = -half_size.x;
			if (lowest_p.y > -half_size.y) lowest_p.y = -half_size.y;
			if (lowest_p.z > -half_size.z) lowest_p.z = -half_size.z;

			if (highest_p.x < half_size.x) highest_p.x = half_size.x;
			if (highest_p.y < half_size.y) highest_p.y = half_size.y;
			if (highest_p.z < half_size.z) highest_p.z = half_size.z;
		}
	}

	float4x4 inh_transform = ((ComponentTransform*)getComponent(TRANSFORM))->getInheritedTransform();

	if (lowest_p.Equals(float3::inf) || highest_p.Equals(-float3::inf))
		centroid = float3::zero;
	else
		centroid = float3(((lowest_p + highest_p) * 0.5f) + inh_transform.TranslatePart());

	float3 inh_scale = inh_transform.GetScale();
	half_size = highest_p;
	half_size = { half_size.x * inh_scale.x, half_size.y * inh_scale.y , half_size.z * inh_scale.z };
}



void GameObject::getInheritedHalfsizeAndCentroid(float3& out_half_size, float3& out_centroid)
{
	std::list<GameObject*> children;
	getChildren(children);

	float3 lowest_p = centroid - half_size;
	float3 highest_p = centroid + half_size;

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		ComponentTransform* transform = (ComponentTransform*)(*it)->getComponent(TRANSFORM);
		math::OBB temp_obb;

		temp_obb.axis[0] = transform->Right();
		temp_obb.axis[1] = transform->Up();
		temp_obb.axis[2] = transform->Forward();
		temp_obb.r = (*it)->getHalfsize();
		temp_obb.pos = float3::zero;

		float3 corners[8];
		temp_obb.GetCornerPoints(corners);

		for (int i = 0; i < 8; i++)
		{
			if (lowest_p.x > ((*it)->getCentroid().x - corners[i].x))  lowest_p.x = (*it)->getCentroid().x - corners[i].x;
			if (lowest_p.y > ((*it)->getCentroid().y - corners[i].y))  lowest_p.y = (*it)->getCentroid().y - corners[i].y;
			if (lowest_p.z > ((*it)->getCentroid().z - corners[i].z))  lowest_p.z = (*it)->getCentroid().z - corners[i].z;

			if (highest_p.x < ((*it)->getCentroid().x + corners[i].x)) highest_p.x = (*it)->getCentroid().x + corners[i].x;
			if (highest_p.y < ((*it)->getCentroid().y + corners[i].y)) highest_p.y = (*it)->getCentroid().y + corners[i].y;
			if (highest_p.z < ((*it)->getCentroid().z + corners[i].z)) highest_p.z = (*it)->getCentroid().z + corners[i].z;

		}
	}

	out_centroid = (lowest_p + highest_p) * 0.5f;
	out_half_size = highest_p - centroid;

}
