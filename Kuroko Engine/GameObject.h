#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_
#include <list>
#include <string>
#include "Component.h"
#include "Globals.h"
#include "Vector3.h"

class GameObject
{
	friend class ComponentTransform;
private:

	std::list<Component*> components;
	std::list<GameObject*> children;
	std::string name;

	GameObject* parent = nullptr;

	Vector3f centroid = Vector3f::Zero;
	Vector3f half_size = Vector3f::Zero;

	uint id = 0;

public:

	bool is_active = true;
	bool is_static = false;

public:

	GameObject(const char* name, GameObject* parent = nullptr);
	~GameObject();

	bool Update(float dt);

	void addComponent(Component* component);
	Component* addComponent(Component_type type);
	void removeComponent(Component* component);

	Component* getComponent(Component_type type);
	bool getComponents(Component_type type, std::list<Component*>& list_to_fill);
	void getComponents(std::list<Component*>& list_to_fill) const { list_to_fill = components; };

	GameObject* getChild(const char* name);
	void getChildren(std::list<GameObject*>& list_to_fill) { list_to_fill = children; };
	void addChild(GameObject* new_child) {if(new_child) children.push_back(new_child); };

	GameObject* getParent() const { return parent; };
	void setParent(GameObject* parent) { this->parent = parent; }// Can recieve nullptr

	Vector3f getCentroid() const { return centroid; };
	Vector3f getHalfsize() const { return half_size; };

	void getInheritedHalfsizeAndCentroid(Vector3f& half_size, Vector3f& centroid);

	bool isActive() const { return is_active; }
	bool isStatic() const { return is_static; }
	void setActive(bool state) { is_active = state; }
	void setStatic(bool state) { is_static = state; }

	void Rename(const char* new_name) { name = new_name; };
	std::string getName() { return name; };

private: 
	void calculateCentroidandHalfsize();
};
#endif