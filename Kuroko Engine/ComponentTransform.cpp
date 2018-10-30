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

	JSON_Value* local_trans = json_value_init_object();
	JSON_Value* global_trans = json_value_init_object();
	JSON_Value* transform = json_value_init_object();
	// Set component type

	json_object_set_string(&config, "type", "transform");
	// Store local transform
	// Position
	json_object_set_number(json_object(local_trans), "px", local->position.x);
	json_object_set_number(json_object(local_trans), "py", local->position.y);
	json_object_set_number(json_object(local_trans), "pz", local->position.z);
	// Scale
	json_object_set_number(json_object(local_trans), "sx", local->scale.x);
	json_object_set_number(json_object(local_trans), "sy", local->scale.y);
	json_object_set_number(json_object(local_trans), "sz", local->scale.z);
	// Rotation
	json_object_set_number(json_object(local_trans), "qx", local->rotation.x);
	json_object_set_number(json_object(local_trans), "qy", local->rotation.y);
	json_object_set_number(json_object(local_trans), "qz", local->rotation.z);
	json_object_set_number(json_object(local_trans), "qw", local->rotation.w);

	// Store global transform
	// Position
	json_object_set_number(json_object(global_trans), "px", local->position.x);
	json_object_set_number(json_object(global_trans), "py", local->position.y);
	json_object_set_number(json_object(global_trans), "pz", local->position.z);
	// Scale
	json_object_set_number(json_object(global_trans), "sx", local->scale.x);
	json_object_set_number(json_object(global_trans), "sy", local->scale.y);
	json_object_set_number(json_object(global_trans), "sz", local->scale.z);
	// Rotation
	json_object_set_number(json_object(global_trans), "qx", local->rotation.x);
	json_object_set_number(json_object(global_trans), "qy", local->rotation.y);
	json_object_set_number(json_object(global_trans), "qz", local->rotation.z);
	json_object_set_number(json_object(global_trans), "qw", local->rotation.w);

	json_object_set_value(json_object(transform), "local", local_trans);
	json_object_set_value(json_object(transform), "global", global_trans);
	json_object_set_value(&config, "transform", transform);

}

void ComponentTransform::Load(JSON_Object & config) {

}

