#ifndef __MODULE_IMGUI
#define __MODULE_IMGUI
#include "Module.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui\ImGuizmo.h"
#include "ComponentCamera.h"
#include "FileSystem.h"
#include <array>
#include "ImGui/TextEditor.h"

struct ImGuiIO;
class GameObject;
class Component;
class Texture;
class Material;
class Camera;
struct Color;
class UI_Element;
class UI_SceneTab;

enum UI_Type { UI_UNKNOWN, UI_HIERARCHY, UI_OBJ_INSPECTOR, UI_PRIMITIVE, UI_GRAPHICS, UI_ABOUT, LOG, TIME_CONTROL, CONFIGURATION, COLOR_PICKER, UI_WINDOW_CONFIG,
				UI_HARDWARE, UI_APPLICATION, UI_SCENE, UI_GAME,
				QUADTREE_CONFIG, CAMERA_MENU, VIEWPORT_MENU /*AUDIO,*/, ASSET_WINDOW, RESOURCES_TAB, SKYBOX_MENU, SCRIPT_EDITOR, LAST_UI_TAB };  


enum UI_textures { NO_TEXTURE, PLAY, PAUSE, STOP, ADVANCE, GUIZMO_TRANSLATE, GUIZMO_ROTATE, GUIZMO_SCALE, GUIZMO_LOCAL, GUIZMO_GLOBAL, 
					GUIZMO_SELECT, FOLDER_ICON, OBJECT_ICON, SCENE_ICON, SCRIPT_ICON, PREFAB_ICON, RETURN_ICON, CAUTION_ICON,
					WARNING_ICON, LAST_UI_TEX};

enum UI_Font {REGULAR, REGULAR_BOLD, REGULAR_ITALIC, REGULAR_BOLDITALIC, TITLES, IMGUI_DEFAULT, LAST_UI_FONT};

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
	void InitializeScriptEditor();

	void DrawHierarchyTab();     // renewal done
	bool DrawHierarchyNode(GameObject& game_object, int& id); // renewal done
	void DrawObjectInspectorTab(); // renewal done
	bool DrawComponent(Component& component, int id); // renewal done
	//void DrawAudioTab();
	void DrawPrimitivesTab(); // renewal done
	void DrawGraphicsLeaf() const;  // renewal done
	void DrawAboutLeaf(); // renewal done
	void DrawWindowConfigLeaf() const; // renewal done
	void DrawHardwareLeaf() const;  // renewal done
	void DrawApplicationLeaf() const;  // renewal done
	void DrawEditorPreferencesLeaf() const;  // renewal done
	void DrawTimeControlWindow();
	void DrawCameraViewWindow(Camera& camera);
	void DrawGizmoMenuTab();
	void DrawQuadtreeConfigWindow();
	void DrawCameraMenuWindow();
	void DrawViewportsWindow();
	void DrawAssetsWindow();
	void DrawAssetInspector();
	void DrawResourcesWindow(); // A list where you can see all the resources
	void DrawSkyboxWindow();
	void DrawColorPickerWindow(const char* label, Color* color, bool* closing_bool, Color* ref_color = nullptr);
	void DrawScriptEditor();

	void DrawGuizmo();
	void DrawTagSelection(GameObject* object);

	void InvisibleDockingBegin();
	void InvisibleDockingEnd();

	void MouseOnScene(bool mouseScene) {mouseOnScene = mouseScene;}; //USED FOR USING MOUSE ON SCENE (MOUSEWHEEL, ETC..)
	void MouseOnGame(bool mouseGame) { mouseOnGame = mouseGame; };
	bool isMouseOnScene() { return mouseOnScene; };
	bool isMouseOnGame() { return mouseOnGame; };

	void setActiveUI_Element(UI_Type type, bool active); // TODO
	UI_Element* getUI_Element(UI_Type type);  // TODO

	UI_SceneTab* scene; //TO ERASE

	void SaveConfig(JSON_Object* config) const;
	void LoadConfig(const JSON_Object* config);

	bool disable_keyboard_control = false;
	std::array<ImFont*, LAST_UI_FONT> ui_fonts;
	std::array<Texture*, LAST_UI_TEX> ui_textures;
	TextEditor script_editor;
	std::string open_script_path;

	
private:
	
	bool docking_background = true;
	bool draw_guizmo = true;

	ImGuiIO* io;
	ImGuizmo::OPERATION	gizmo_operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE gizmo_mode = ImGuizmo::WORLD;

	bool open_tabs[LAST_UI_TAB];  // _serializable_var

	std::string asset_window_path = ASSETS_FOLDER;
	std::string selected_asset;

	std::list<UI_Element*> ui_elements;

	//bools for checking if mouse on scene or game
	bool mouseOnScene = false;
	bool mouseOnGame = false;
};
#endif

