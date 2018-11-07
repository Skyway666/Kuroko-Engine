#ifndef __MODULE_IMGUI
#define __MODULE_IMGUI
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui\ImGuizmo.h"
#include "ComponentCamera.h"
#include <array>

struct ImGuiIO;
class GameObject;
class Component;
class Texture;
class Material;
class Camera;

enum GUI_Tabs { HIERARCHY, OBJ_INSPECTOR, PRIMITIVE, ABOUT, LOG, TIME_CONTROL, CONFIGURATION, /*AUDIO,*/ LAST_UI_TAB};  // LAST is an utility value to store the max num of tabs.
enum UI_textures { PLAY, PAUSE, STOP, ADVANCE, TRANSLATE, ROTATE, SCALE, NO_TEXTURE, LAST_UI_TEX};
enum UI_Fonts {REGULAR, REGULAR_BOLD, REGULAR_ITALIC, REGULAR_BOLDITALIC, TITLES, LAST_UI_FONT};

class ModuleUI :
	public Module {
public:
	ModuleUI(Application* app, bool start_enabled = true);
	~ModuleUI();

	bool Init(const JSON_Object* config);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void DrawGraphicsTab() const;
	void DrawHierarchyTab();
	void DrawHierarchyNode(const GameObject& game_object, int& id) const;
	void DrawObjectInspectorTab();
	bool DrawComponent(Component& component);
	//void DrawAudioTab();
	void DrawPrimitivesTab();
	void DrawAboutWindow();
	void DrawWindowConfig() const;
	void DrawHardware() const;
	void DrawApplication() const;
	void DrawTimeControl();
	void DrawCameraView(const ComponentCamera& camera);
	void DrawAndSetGizmoOptions();
	void DrawGuizmo();

	void InvisibleDockingBegin();
	void InvisibleDockingEnd();

	void SaveConfig(JSON_Object* config) const;
	void LoadConfig(const JSON_Object* config);

private:
	
	bool close_app = false;
	bool docking_background;
	bool open_tabs[LAST_UI_TAB];  // _serializable_var
	ImGuiIO* io;
	std::array<Texture*, LAST_UI_TEX> ui_textures;
	std::array<ImFont*, LAST_UI_FONT> ui_fonts;
	ImGuizmo::OPERATION	gizmo_operation = ImGuizmo::TRANSLATE;
};
#endif

