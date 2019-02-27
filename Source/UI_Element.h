#ifndef _UI_ELEMENT_
#define _UI_ELEMENT_

#include "ImGui/imgui.h"
#include "Application.h"
#include "ModuleUI.h"


class UI_Element
{
public:
	UI_Element(const char* _name, UI_Type _type, UI_Font _font_type) : name(_name), type(_type)
	{
		font = App->gui->ui_fonts[_font_type];
	};
	virtual ~UI_Element() {};

	void Update() {
		OpenLayout(); CloseLayout();
	};

	UI_Type getType() { return type; };
	void setFont(UI_Font font_type) { font = App->gui->ui_fonts[font_type]; };

protected:
	virtual void OpenLayout() {};
	virtual void UpdateContent() {};
	virtual void CloseLayout() {};

	virtual bool isWindow() { return false; };
	virtual bool isTab() { return false; };
	virtual bool isLeaf() { return false; };

	UI_Type type = UI_UNKNOWN;
	const char* name = nullptr;
	bool active = true;
	ImFont* font = nullptr;
};

#endif