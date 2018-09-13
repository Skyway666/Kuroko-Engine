#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentAABB.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	addComponent(TRANSFORM);
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
			APPLOG("error in gameobject %s", name.c_str());
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
	switch (component->getType())
	{
	case MESH:	
		components.push_back(component);
		calculateCentroidandHalfsize();
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

	Vector3f lowest_p = Vector3f::PosInfinity;
	Vector3f highest_p = Vector3f::NegInfinity;

	for (std::list<Component*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		ComponentMesh* mesh = (ComponentMesh*)(*it);

		if (lowest_p.x > (mesh->centroid.x - mesh->half_size.x)) lowest_p.x = mesh->centroid.x - mesh->half_size.x;
		if (lowest_p.y > (mesh->centroid.y - mesh->half_size.y)) lowest_p.y = mesh->centroid.y - mesh->half_size.y;
		if (lowest_p.z > (mesh->centroid.z - mesh->half_size.z)) lowest_p.z = mesh->centroid.z - mesh->half_size.z;

		if (highest_p.x < (mesh->centroid.x + mesh->half_size.x)) highest_p.x = mesh->centroid.x + mesh->half_size.x;
		if (highest_p.y < (mesh->centroid.y + mesh->half_size.y)) highest_p.y = mesh->centroid.y + mesh->half_size.y;
		if (highest_p.z < (mesh->centroid.z + mesh->half_size.z)) highest_p.z = mesh->centroid.z + mesh->half_size.z;
	}

	centroid = ((lowest_p + highest_p) * 0.5f);
	half_size = highest_p - centroid;

}

Vector3f GameObject::getInheritedCenter()
{
	std::list<GameObject*> children;
	getChildren(children);

	Vector3f lowest_p = centroid - half_size;
	Vector3f highest_p = centroid + half_size;

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if (lowest_p.x > ((*it)->getCentroid().x - (*it)->getHalfsize().x)) lowest_p.x = (*it)->getCentroid().x - (*it)->getHalfsize().x;
		if (lowest_p.y > ((*it)->getCentroid().y - (*it)->getHalfsize().y)) lowest_p.y = (*it)->getCentroid().y - (*it)->getHalfsize().y;
		if (lowest_p.z > ((*it)->getCentroid().z - (*it)->getHalfsize().z)) lowest_p.z = (*it)->getCentroid().z - (*it)->getHalfsize().z;

		if (highest_p.x < ((*it)->getCentroid().x + (*it)->getHalfsize().x)) highest_p.x = (*it)->getCentroid().x + (*it)->getHalfsize().x;
		if (highest_p.y < ((*it)->getCentroid().y + (*it)->getHalfsize().y)) highest_p.y = (*it)->getCentroid().y + (*it)->getHalfsize().y;
		if (highest_p.z < ((*it)->getCentroid().z + (*it)->getHalfsize().z)) highest_p.z = (*it)->getCentroid().z + (*it)->getHalfsize().z;
	}
	
	ComponentTransform* transform = (ComponentTransform*)getComponent(TRANSFORM);
	float3 inh_pos = transform->getInheritedTransform().TranslatePart();
	return ((lowest_p + highest_p) * 0.5f) + Vector3f(inh_pos.x, inh_pos.y, inh_pos.z) - centroid;
}

Vector3f  GameObject::getInheritedCentroid()
{
	std::list<GameObject*> children;
	getChildren(children);

	Vector3f lowest_p = centroid - half_size;
	Vector3f highest_p = centroid + half_size;

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if (lowest_p.x > ((*it)->getCentroid().x - (*it)->getHalfsize().x)) lowest_p.x = (*it)->getCentroid().x - (*it)->getHalfsize().x;
		if (lowest_p.y > ((*it)->getCentroid().y - (*it)->getHalfsize().y)) lowest_p.y = (*it)->getCentroid().y - (*it)->getHalfsize().y;
		if (lowest_p.z > ((*it)->getCentroid().z - (*it)->getHalfsize().z)) lowest_p.z = (*it)->getCentroid().z - (*it)->getHalfsize().z;

		if (highest_p.x < ((*it)->getCentroid().x + (*it)->getHalfsize().x)) highest_p.x = (*it)->getCentroid().x + (*it)->getHalfsize().x;
		if (highest_p.y < ((*it)->getCentroid().y + (*it)->getHalfsize().y)) highest_p.y = (*it)->getCentroid().y + (*it)->getHalfsize().y;
		if (highest_p.z < ((*it)->getCentroid().z + (*it)->getHalfsize().z)) highest_p.z = (*it)->getCentroid().z + (*it)->getHalfsize().z;
	}
	ComponentTransform* transform = (ComponentTransform*)getComponent(TRANSFORM);

	return ((lowest_p + highest_p) * 0.5f);
}


Vector3f GameObject::getInheritedHalfsize()
{
	std::list<GameObject*> children;
	getChildren(children);

	Vector3f lowest_p = centroid - half_size;
	Vector3f highest_p = centroid + half_size;

	for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
	{
		if (lowest_p.x > ((*it)->getCentroid().x - (*it)->getHalfsize().x)) lowest_p.x = (*it)->getCentroid().x - (*it)->getHalfsize().x;
		if (lowest_p.y > ((*it)->getCentroid().y - (*it)->getHalfsize().y)) lowest_p.y = (*it)->getCentroid().y - (*it)->getHalfsize().y;
		if (lowest_p.z > ((*it)->getCentroid().z - (*it)->getHalfsize().z)) lowest_p.z = (*it)->getCentroid().z - (*it)->getHalfsize().z;

		if (highest_p.x < ((*it)->getCentroid().x + (*it)->getHalfsize().x)) highest_p.x = (*it)->getCentroid().x + (*it)->getHalfsize().x;
		if (highest_p.y < ((*it)->getCentroid().y + (*it)->getHalfsize().y)) highest_p.y = (*it)->getCentroid().y + (*it)->getHalfsize().y;
		if (highest_p.z < ((*it)->getCentroid().z + (*it)->getHalfsize().z)) highest_p.z = (*it)->getCentroid().z + (*it)->getHalfsize().z;
	}

	return (highest_p - ((lowest_p + highest_p) * 0.5f));
}


Vector3f GameObject::getCenter()
{
	Vector3f center = centroid;
	ComponentTransform* transform = (ComponentTransform*)getComponent(TRANSFORM);
	float3 inh_pos = transform->getInheritedTransform().TranslatePart();
	center += Vector3f(inh_pos.x, inh_pos.y, inh_pos.z);

	return center;
}