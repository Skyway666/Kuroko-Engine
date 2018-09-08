#pragma once

class GameObject;

enum Component_type { NONE, MESH, TRANSFORM };

class Component
{
public:

	Component(GameObject* gameobject, Component_type type) : parent(gameobject), type(type) {};

	virtual bool Update(float dt) { return true; };

	bool isActive() { return is_active; }
	void setActive(bool state) { is_active = state; }

	Component_type getType() { return type; };
	GameObject* getParent() { return parent; };
	void setParent(GameObject* new_parent) { parent = new_parent; }

	bool is_active = true;
private:

	Component_type type = NONE;
	GameObject* parent = nullptr;
};