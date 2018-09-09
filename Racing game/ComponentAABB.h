#pragma once

#include "Vector3.h"
#include "Component.h"

#include <list>

class ComponentTransform;

class ComponentAABB : public Component
{
public:
	ComponentAABB(GameObject* parent);
	~ComponentAABB();

	void DrawAABB();
	void getAllMeshes(GameObject* obj, std::list<Component*>& list_to_fill);
	bool Update(float dt);
	math::AABB* getAABB() { return aabb; };
	math::OBB* getOBB() { return obb; };

public:
	bool draw = false;

private: 

	math::AABB* aabb = nullptr;
	math::OBB* obb = nullptr;

	ComponentTransform* transform = nullptr;
	Vector3f last_pos = { 0.0f, 0.0f, 0.0f };
	Vector3f last_scl = { 0.0f, 0.0f, 0.0f };
	Vector3f last_rot = { 0.0f, 0.0f, 0.0f };
};