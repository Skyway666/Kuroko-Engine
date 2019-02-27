#ifndef __UI_WINDOW_
#define __UI_WINDOW_

#include "UI_Element.h"

class UI_Leaf : public UI_Element  // use Leaf for elements who work as collapsing headers inside a window
{
protected:

	UI_Leaf(const char* _name, UI_Type _type, UI_Font _font_type) : UI_Element(_name, _type, REGULAR) {};

	void OpenLayout() {
		if (active = ImGui::CollapsingHeader(name))
			{ UpdateContent(); ImGui::PushFont(font); }
	};
	void CloseLayout() { if(active) ImGui::PopFont(); };

	bool isLeaf() { return true; }
};


#endif