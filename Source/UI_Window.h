#ifndef __UI_WINDOW_
#define __UI_WINDOW_

#include "UI_Element.h"

class UI_Window : public UI_Element   // used for elements who have their own window
{
protected:
	UI_Window(const char* _name, UI_Type _type, UI_Font _font_type = REGULAR) : UI_Element(_name, _type, _font_type) {};
	void OpenLayout() { ImGui::Begin(name, &active); ImGui::PushFont(font); UpdateContent(); };
	void CloseLayout() { ImGui::PopFont(); ImGui::End(); };

	void setPosition(int x, int y) { ImGui::SetWindowPos(ImVec2(x, y)); };
	void setSize(int x, int y) { ImGui::SetWindowSize(ImVec2(x, y)); };

	bool isWindow() { return true; }
	bool isTab() { return true; }

	void Activate() { active = true; };
	void Deactivate() { active = false; };
};

typedef UI_Window UI_Tab;  // use UI_Tab when you want to specify that an element must be tabbed in its default form

#endif