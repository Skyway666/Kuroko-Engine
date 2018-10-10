#ifndef _COMPONENT_
#define _COMPONENT_
class GameObject;

enum Component_type { NONE, MESH, TRANSFORM, C_AABB };

class Component
{
public:

	Component(GameObject* gameobject, Component_type type) : parent(gameobject), type(type) {};

	virtual bool Update(float dt) { return true; };

	bool isActive() { return is_active; } const
	void setActive(bool state) { is_active = state; }

	Component_type getType() const { return type; };
	GameObject* getParent() const { return parent; };
	void setParent(GameObject* new_parent) { parent = new_parent; } // Can recieve nullptr

private:

	bool is_active = true;
	Component_type type = NONE;
	GameObject* parent = nullptr;
};
#endif