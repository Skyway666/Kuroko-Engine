#include "UI_SceneTab.h"
#include "ModuleCamera3D.h"
#include "Camera.h"
#include "Material.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ModuleRenderer3D.h"

void UI_SceneTab::UpdateContent()
{

	pos = float2(ImGui::GetCursorPosX() + ImGui::GetWindowPos().x, ImGui::GetCursorPosY() + ImGui::GetWindowPos().y);

	size = float2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

	if (lastSize.x != size.x || lastSize.y != size.y)
	{
		lastSize.x = size.x; lastSize.y = size.y;
		
		App->renderer3D->OnResize2(size.x, size.y, true);
	}

	/*img = (ImTextureID)App->camera->selected_camera;*/
	FrameBuffer* frame_buffer = App->camera->current_camera->getFrameBuffer();
	img = (ImTextureID)frame_buffer->tex->gl_id;
	ImGui::Image(img, ImVec2(size.x, size.y), ImVec2(0, 1), ImVec2(1, 0));
	App->gui->MouseOnScene(ImGui::IsMouseHoveringWindow());
	//App->scene->selected_obj = App->scene->MousePicking(App->camera->current_camera->getParent() ? App->camera->current_camera->getParent()->getParent() : nullptr);

}
