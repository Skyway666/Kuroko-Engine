#include "ComponentCamera.h"
#include "Application.h"
#include "Camera.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "ModuleDebug.h"

#include "MathGeoLib\Math\Quat.h"

ComponentCamera::ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, CAMERA), camera(camera)
{
	transform = (ComponentTransform*)parent->getComponent(TRANSFORM);
	camera->attached_to = this;
}

bool ComponentCamera::Update(float dt)
{
	if (App->camera->selected_camera != camera)
	{
		float3 displacement = camera->getFrustum()->pos;
		camera->getFrustum()->pos = transform->local->getPosition() + offset;
		displacement = camera->getFrustum()->pos - displacement;
		camera->Reference += displacement;
		camera->Reference = camera->getFrustum()->pos + (transform->local->Forward().Normalized() * (camera->Reference - camera->getFrustum()->pos).Length());
		camera->X = transform->local->Right();
		camera->Y = transform->local->Up();
		camera->Z = transform->local->Forward();
		camera->updateFrustum();
	}

	getCamera()->active = getCamera()->draw_in_UI;

	return true;
}

void ComponentCamera::Draw() const
{
	if (getCamera()->draw_frustum)
		App->debug->directDrawFrustum(*getCamera()->getFrustum());
}