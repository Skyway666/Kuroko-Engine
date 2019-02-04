#include "ComponentAnimation.h"
#include "Application.h"
#include "ModuleResourcesManager.h"

ComponentAnimation::ComponentAnimation(JSON_Object* deff, GameObject* parent): Component(parent, ANIMATION)
{
	loop = json_object_get_boolean(deff, "loop");
	speed = json_object_get_number(deff, "speed");

	const char* parent3dobject = json_object_get_string(deff, "Parent3dObject");
	if (parent3dobject) // Means that is being loaded from a scene
		animation_resource_uuid = App->resources->getMeshResourceUuid(parent3dobject, json_object_get_string(deff, "animation_name")); //SKELETAL_TODO: getAnimationResourceUuid()
	else // Means it is being loaded from a 3dObject binary
		animation_resource_uuid = json_object_get_number(deff, "animation_resource_uuid");

	App->resources->assignResource(animation_resource_uuid);
}

ComponentAnimation::~ComponentAnimation()
{
}

bool ComponentAnimation::Update(float dt)
{
	return true;
}

void ComponentAnimation::Save(JSON_Object * config)
{
	json_object_set_string(config, "type", "animation");

	json_object_set_boolean(config, "loop", loop);
	json_object_set_number(config, "speed", speed);

	if (animation_resource_uuid != 0)
	{
		/*ResourceAnimation* res_anim = (ResourceAnimation*)App->resources->getResource(animation_resource_uuid);
		if (res_anim)
		{
			json_object_set_string(config, "animation_name", res_anim->asset.c_str());
			json_object_set_string(config, "Parent3dObject", res_anim->Parent3dObject.c_str());
		}
		else
		{*/
		json_object_set_string(config, "animation_name", "missing reference");
		json_object_set_string(config, "Parent3dObject", "missing reference");
		//}
	}
}
