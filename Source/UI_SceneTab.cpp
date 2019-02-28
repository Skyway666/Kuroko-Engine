#include "UI_SceneTab.h"
#include "ModuleCamera3D.h"
#include "Camera.h"
#include "Material.h"

void UI_SceneTab::UpdateContent()
{
	uint flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;


	pos = float2(ImGui::GetCursorPosX() + ImGui::GetWindowPos().x, ImGui::GetCursorPosY() + ImGui::GetWindowPos().y);

	size = float2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

	if (lastSize.x != size.x || lastSize.y != size.y)
	{
		lastSize.x = size.x; lastSize.y = size.y;
		
		//App->renderer3D->OnResize(size.x, size.y, true);
	}

	/*img = (ImTextureID)App->camera->selected_camera;*/
	FrameBuffer* frame_buffer = App->camera->selected_camera->getFrameBuffer();
	img = (ImTextureID)frame_buffer->tex->gl_id;
	ImGui::Image(img, ImVec2(size.x, size.y), ImVec2(0, 1), ImVec2(1, 0));

}
