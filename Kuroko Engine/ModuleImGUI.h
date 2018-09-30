#pragma once
#include "Module.h"
#include "Globals.h"
#include "imgui.h"
#include <array>

struct ImGuiIO;
class GameObject;
class Component;
class Material;

enum GUI_Tabs { DEMO, GRAPHIC, TEST, HIERARCHY, OBJ_INSPECTOR, PRIMITIVE, IMPORTER, ABOUT, WINDOW_CONFIG, HARDWARE, APPLICATION, LOG, TIME_CONTROL, LAST};  // LAST is an utility value to store the max num of tabs.
enum UI_textures { PLAY, PAUSE, STOP, LAST_TEX};

class ModuleImGUI :
	public Module {
public:
	ModuleImGUI(Application* app, bool start_enabled = true);
	~ModuleImGUI();

	bool Init(JSON_Object* config);
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
	void DrawAboutWindow();
	void DrawWindowConfig();
	void DrawHardware();
	void DrawApplication();
	void DrawTimeControl();

	void SaveConfig(JSON_Object* config);
	void LoadConfig(JSON_Object* config);

private:
	
	bool open_tabs[LAST];  // _serializable_var
	ImGuiIO io;
	std::array<Material*, LAST_TEX> ui_textures;
};

