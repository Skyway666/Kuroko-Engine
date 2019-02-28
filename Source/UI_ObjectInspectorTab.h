#ifndef __UI_OBJECT_INSPECTOR_TAB_
#define __UI_OBJECT_INSPECTOR_TAB_

#include "UI_Window.h"
#include "Component.h"

class GameObject;

class UI_ObjectInspectorTab : public UI_Tab
{
public:
	UI_ObjectInspectorTab() : UI_Tab("Object inspector", UI_OBJ_INSPECTOR) {};
	void UpdateContent();

private:
	bool DrawComponent(Component& component);
	void DrawTagSelection(GameObject* object);

	GameObject* selected_object = nullptr;
	uint current_id = 0;
};


#endif