#pragma once

class GameObject;

enum Component_type { NONE, MESH };

class Component
{
public:

	Component(GameObject* gameobject) : parent(gameobject) {};

	virtual bool Update(float dt) { return false; };

	bool isActive() { return is_active; }
	void setActive(bool state) { is_active = state; }

	Component_type getType() { return type; };
	GameObject* getParent() { return parent; };
	void setParent(GameObject* new_parent) { parent = new_parent; }

private:

	Component_type type = NONE;
	GameObject* parent = nullptr;
	bool is_active = true;
};