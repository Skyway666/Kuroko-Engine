#include "ComponentTransform.h"
#include "GameObject.h"
#include "Transform.h"

#include "glew-2.1.0\include\GL\glew.h"

ComponentTransform::ComponentTransform(GameObject* parent, const Quat& rot, const float3& pos, const float3& scl) : Component(parent, TRANSFORM)
{
	local = new Transform();
	global = new Transform();

	local->setPosition(pos);
	local->setRotation(rot);
	local->setScale(scl);
	local->CalculateMatrix();

	LocalToGlobal();
}

ComponentTransform::ComponentTransform(GameObject* parent, const float3& euler_axis, const float3& pos, const float3& scl) : Component(parent, TRANSFORM)
{
	local = new Transform();
	global = new Transform();

	local->setPosition(pos);
	local->setRotation(Quat::FromEulerXYZ(euler_axis.x, euler_axis.y, euler_axis.z));
	local->setScale(scl);
	local->CalculateMatrix();

	LocalToGlobal();
}

ComponentTransform::ComponentTransform(GameObject* parent, const ComponentTransform& transform) : Component(parent, TRANSFORM)
{
	local = new Transform(*transform.local);
	global = new Transform(*transform.global);
}

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent, TRANSFORM)
{
	local = new Transform();
	global = new Transform();
}

ComponentTransform::~ComponentTransform()
{
	if (local) delete local;
	if (global) delete global;
}

bool ComponentTransform::Update(float dt)
{
	if (mode == GLOBAL)
	{
		global->CalculateMatrix();
		GlobalToLocal();
	}
	else
	{
		local->CalculateMatrix();
		LocalToGlobal();
	}

	return true;
}

Transform* ComponentTransform::getInheritedTransform()
{
	if (GameObject* parent_obj = getParent()->getParent())
		return ((ComponentTransform*)parent_obj->getComponent(TRANSFORM))->local;
	else 
		return nullptr;
}

void ComponentTransform::GlobalToLocal()
{
	if (Transform* inh_transform = getInheritedTransform())
	{
		local->setPosition(global->getPosition() - inh_transform->getPosition() - (local->getRotation() * inh_transform->getRotation() * getParent()->own_centroid));
		local->setRotation(global->getRotation() * inh_transform->getRotation().Inverted());
		local->setScale(global->getScale().Div(inh_transform->getScale()));
	}
	else
		local->Set(global->getPosition(), global->getRotation(), global->getScale());

	local->CalculateMatrix();
}

void ComponentTransform::LocalToGlobal()
{
	if (Transform* inh_transform = getInheritedTransform())
	{
		global->setPosition(local->getPosition() + inh_transform->getPosition() + (local->getRotation() * inh_transform->getRotation() * getParent()->own_centroid));
		global->setRotation(local->getRotation() * inh_transform->getRotation());
		global->setScale(local->getScale().Mul(inh_transform->getScale()));
	}
	else
		global->Set(local->getPosition(), local->getRotation(), local->getScale());

	global->CalculateMatrix();
}

void ComponentTransform::Save(JSON_Object & config) {

}

void ComponentTransform::Load(JSON_Object & config) {

}

