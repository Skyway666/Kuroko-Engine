#ifndef __UI_COLORPICKER_TAB_
#define __UI_COLORPICKER_TAB_

#include "UI_Window.h"

class UI_ColorPickerWindow : public UI_Tab
{
public:
	UI_ColorPickerWindow() : UI_Tab("Color Picker", COLOR_PICKER) {};
	void Draw(const char* label, Color* color, bool* closing_bool, Color* ref_color);

private:
};


#endif