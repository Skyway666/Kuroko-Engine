#ifndef __UI_HIERARCHY_TAB_
#define __UI_HIERARCHY_TAB_

#include "UI_Window.h"

class UI_HierarchyTab : public UI_Tab 
{
public:
	UI_HierarchyTab() : UI_Tab("Hierarchy", UI_HIERARCHY) {};
	void UpdateContent();

private:
	bool DrawHierarchyNode(GameObject& game_object);

	uint current_id = 0;
};


#endif