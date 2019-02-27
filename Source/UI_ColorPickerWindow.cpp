#include "UI_ColorPickerWindow.h"
#include "Color.h"

void UI_ColorPickerWindow::Draw(const char* label, Color* color, bool* closing_bool, Color* ref_color = nullptr)
{
	ImGui::Text("Use right click to change color picker mode");
	ImGui::ColorPicker4(name, &color->r, 0, &ref_color->r);
}