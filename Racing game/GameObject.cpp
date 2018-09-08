#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

GameObject::GameObject(const char* name, GameObject* parent) : name(name), parent(parent)
{
	addComponent((Component*)new ComponentTransform(this));
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
		components.push_back(new ComponentMesh(this));
		break;
	default:
		break;
	}

	return new_component;
}
