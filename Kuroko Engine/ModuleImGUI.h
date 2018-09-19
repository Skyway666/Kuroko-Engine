#pragma once
#include "Module.h"
#include "Globals.h"
#include "imgui.h"

struct ImGuiIO;
class GameObject;
class Component;

enum GUI_Tabs { DEMO, GRAPHIC, TEST, HIERARCHY, OBJ_INSPECTOR, PRIMITIVE, IMPORTER, LAST};  // LAST is an utility value to store the max num of tabs.

class ModuleImGUI :
	public Module {
public:
	ModuleImGUI(Application* app, bool start_enabled = true);
	~ModuleImGUI();

	bool Init();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DrawGraphicsTab();
	void DrawHierarchyTab();
	void DrawHierarchyNode(GameObject* game_object, int& id);
	void DrawObjectInspectorTab();
	bool DrawComponent(Component* component);
	void DrawPrimitivesTab();
	void DrawImporterTab();

private:

	void ShowAboutWindow();
	bool show_engine_about = false;
	bool open_tabs[LAST];
	ImGuiIO io;


	// RANDOM TESTING
	int bound1, bound2;
	 
};

