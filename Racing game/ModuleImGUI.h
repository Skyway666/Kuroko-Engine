#pragma once
#include "Module.h"
#include "Globals.h"
#include "imgui.h"

struct ImGuiIO;
struct ImVec4;

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

private:
	bool show_demo_window = false;
	bool show_graphic_tab = true;
	bool show_test_tab = true;
	ImVec4 clear_color;
	ImGuiIO io;
};

