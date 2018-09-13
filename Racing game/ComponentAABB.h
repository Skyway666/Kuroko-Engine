#pragma once

#include "Vector3.h"
#include "Component.h"
#include "Color.h"

#include <list>

class ComponentTransform;

class ComponentAABB : public Component
{
public:
	ComponentAABB(GameObject* parent) : Component(parent, C_AABB) { Reload(); };
	~ComponentAABB();

	bool Update(float dt);
	void Reload();

	math::AABB* getAABB() { return aabb; };
	math::OBB* getOBB() { return obb; };

	void DrawAABB();
	void DrawOBB();

	Vector3f getCentroid();

public:
	bool draw_aabb = false;
	bool draw_obb = false;

private:

	void getAllMeshes(GameObject* obj, std::list<Component*>& list_to_fill);
	float3 Centroid();

private: 

	math::AABB* aabb = nullptr;
	math::OBB* obb = nullptr;

	ComponentTransform* transform = nullptr;
	Vector3f last_pos = { 0.0f, 0.0f, 0.0f };
	Vector3f last_scl = { 0.0f, 0.0f, 0.0f };
	Quat last_rot = { 0.0f, 0.0f, 0.0f, 0.0f };

	float3 mesh_center = { 0.0f, 0.0f, 0.0f };
};