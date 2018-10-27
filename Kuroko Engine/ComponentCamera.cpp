#include "ComponentCamera.h"
#include "Camera.h"
#include "ComponentTransform.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, C_AABB)
{
	transform = (ComponentTransform*)parent->getComponent(TRANSFORM);

	//camera->Position = transform->getInheritedTransform().TranslatePart();
}