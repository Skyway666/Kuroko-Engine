#include "UI_EditorPreferencesLeaf.h"
#include "ModuleCamera3D.h"

void UI_EditorPreferencesLeaf::UpdateContent()
{
	static float camera_speed = 2.5f;
	if (ImGui::InputFloat("Camera speed", &camera_speed))
		App->camera->camera_speed = camera_speed;


	static float camera_rotation_speed = 0.25f;
	if (ImGui::InputFloat("Camera rotation speed", &camera_rotation_speed))
		App->camera->camera_rotation_speed = camera_rotation_speed;
}