#include "UI_WindowConfigLeaf.h"
#include "ModuleWindow.h"

void UI_WindowConfigLeaf::UpdateContent()
{
	Window* window = App->window->main_window;
	if (ImGui::SliderFloat("##Brightness", &window->brightness, 0, 1.0f, "Brightness: %.2f"))
		App->window->setBrightness(window->brightness);

	bool width_mod, height_mod = false;
	width_mod = ImGui::SliderInt("##Window width", &window->width, MIN_WINDOW_WIDTH, MAX_WINDOW_WIDTH, "Width: %d");
	height_mod = ImGui::SliderInt("###Window height", &window->height, MIN_WINDOW_HEIGHT, MAX_WINDOW_HEIGHT, "Height: %d");

	if (width_mod || height_mod)
		App->window->setSize(window->width, window->height);

	// Refresh rate
	ImGui::Text("Refresh Rate %i", (int)ImGui::GetIO().Framerate);
	//Bools
	if (ImGui::Checkbox("Fullscreen", &window->fullscreen))
		App->window->setFullscreen(window->fullscreen);
	ImGui::SameLine();

	if (ImGui::Checkbox("Resizable", &window->resizable))
		App->window->setResizable(window->resizable);

	if (ImGui::Checkbox("Borderless", &window->borderless))
		App->window->setBorderless(window->borderless);

	ImGui::SameLine();
	if (ImGui::Checkbox("FullDesktop", &window->fulldesk))
		App->window->setFullDesktop(window->fulldesk);
}