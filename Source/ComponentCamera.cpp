#include "ComponentCamera.h"
#include "Application.h"
#include "Camera.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"

#include "MathGeoLib\Math\Quat.h"

ComponentCamera::ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, CAMERA), camera(camera)
{
	transform = (ComponentTransform*)parent->getComponent(TRANSFORM);
	camera->attached_to = this;
}

ComponentCamera::ComponentCamera(JSON_Object* deff, GameObject* parent) : Component(parent, CAMERA){

}

ComponentCamera::~ComponentCamera()
{
	if (camera != App->camera->editor_camera && !camera->IsViewport())
	{
		if (camera == App->camera->override_editor_cam_culling) App->camera->override_editor_cam_culling = nullptr;
		if (camera == App->camera->selected_camera)				App->camera->selected_camera = App->camera->background_camera;
		App->camera->game_cameras.remove(camera);
		delete camera;
	}
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

void ComponentCamera::Save(JSON_Object* config) {

	json_object_set_string(config, "type", "camera");

	JSON_Value* transform_value = json_value_init_object();
	transform->Save(json_object(transform_value));

	json_object_set_value(json_object(transform_value), "camera_transform", transform_value);

	JSON_Value* camera_value = json_value_init_object();

	// X
	json_object_set_number(json_object(camera_value), "Xx", camera->X.x);
	json_object_set_number(json_object(camera_value), "Xy", camera->X.y);
	json_object_set_number(json_object(camera_value), "Xz", camera->X.z);

	// Y
	json_object_set_number(json_object(camera_value), "Yx", camera->Y.x);
	json_object_set_number(json_object(camera_value), "Yy", camera->Y.y);
	json_object_set_number(json_object(camera_value), "Yz", camera->Y.z);

	// Z
	json_object_set_number(json_object(camera_value), "Zx", camera->Z.x);
	json_object_set_number(json_object(camera_value), "Zy", camera->Z.y);
	json_object_set_number(json_object(camera_value), "Zz", camera->Z.z);


}