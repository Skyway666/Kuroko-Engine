#include "GameObject.h"
#include "Random.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentAABB.h"
#include "ComponentCamera.h"
#include "Camera.h"
#include "Application.h"
#include "ModuleUI.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "Applog.h"
#include "ModuleRenderer3D.h"


GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent), id(App->scene->last_gobj_id++), uuid(random32bits())
{
	addComponent(TRANSFORM);
	addComponent(C_AABB);
	App->scene->addGameObject(this);
	
}

GameObject::~GameObject()
{
	for (auto it = components.begin(); it != components.end(); it++)
		delete *it;

	for (auto it = children.begin(); it != children.end(); it++)
		(*it)->parent = parent;
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

	//App->renderer3D->DirectDrawCube(float3(10, 10, 10), centroid);

	return ret;
}


Component* GameObject::getComponent(Component_type type) const
{
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); it++)
	{
		if ((*it)->getType() == type)
			return *it;
	}

	return nullptr;
}

bool GameObject::getComponents(Component_type type, std::list<Component*>& list_to_fill) const
{
	for (std::list<Component*>::const_iterator it = components.begin(); it != components.end(); it++)
	{
		if ((*it)->getType() == type)
			list_to_fill.push_back(*it);
	}

	return !list_to_fill.empty();
}

GameObject* GameObject::getChild(const char* name) const
{
	for (std::list<GameObject*>::const_iterator it = children.begin(); it != children.end(); it++)
	{
		if ((*it)->getName() == name)
			return *it;
	}

	return nullptr;
}

void GameObject::getAllDescendants(std::list<GameObject*>& list_to_fill) const
{
	for (auto it = children.begin(); it != children.end(); it++)
		list_to_fill.push_back(*it);

	for (auto it = children.begin(); it != children.end(); it++)
		(*it)->getAllDescendants(list_to_fill);

	return;
}

Component* GameObject::addComponent(Component_type type)
{
	Component* new_component = nullptr;

	switch (type)
	{
	case MESH:		
		new_component = new ComponentMesh(this); 
		components.push_back(new_component);
		((ComponentAABB*)getComponent(C_AABB))->Reload();
		break;
	case C_AABB:	
		if (!getComponent(C_AABB))
		{
			new_component = new ComponentAABB(this);
			components.push_back(new_component);
		}
		break;
	case TRANSFORM:
		if (!getComponent(TRANSFORM))
		{
			new_component = new ComponentTransform(this);
			components.push_back(new_component);
		}
		break;
	case CAMERA:
		if (!getComponent(CAMERA))
		{
			Camera* camera = new Camera();
			new_component = new ComponentCamera(this, camera);
			components.push_back(new_component);
		}
		break;
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
		((ComponentAABB*)getComponent(C_AABB))->Reload();
		break;
	case C_AABB:
		if (!getComponent(C_AABB))
			components.push_back(component);
		break;
	case TRANSFORM:
		if (!getComponent(TRANSFORM))
			components.push_back(component);
		break;
	case CAMERA:
		if (!getComponent(CAMERA))
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
		if (component->getType() == TRANSFORM || component->getType() == C_AABB)
			continue;
		else if (*it == component)
		{
			bool mesh = (component->getType() == MESH);
			components.remove(component);

			if (component->getType() == MESH)
				((ComponentAABB*)getComponent(C_AABB))->Reload();
			
			break;
		}
	}
	
}
