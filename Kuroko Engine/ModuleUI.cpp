 #include "ModuleUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"
#include "ModuleAudio.h"
#include "ModuleTimeManager.h"
#include "ModuleResourcesManager.h"
#include "Applog.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "ImGui/imgui_internal.h"


#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Transform.h"
#include "ComponentAABB.h"
#include "ComponentCamera.h"
#include "Camera.h"
#include "Quadtree.h"
#include "ResourceTexture.h"
#include "ResourceScene.h"

#include "Random.h"
#include "VRAM.h"
#include "WinItemDialog.h" 

#include "Assimp/include/version.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#include <experimental/filesystem>

#pragma comment( lib, "glew-2.1.0/lib/glew32.lib")
#pragma comment( lib, "glew-2.1.0/lib/glew32s.lib")


ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled) {
	name = "gui";
}


ModuleUI::~ModuleUI() {
}

bool ModuleUI::Init(const JSON_Object* config) {
	
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(App->window->main_window->window, App->renderer3D->getContext());
	ImGui_ImplOpenGL2_Init();

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	LoadConfig(config);


	return true;
}

bool ModuleUI::Start()
{
	io = &ImGui::GetIO();

	ui_textures[PLAY]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/Play.png");
	ui_textures[PAUSE]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/Pause.png");
	ui_textures[STOP]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/Stop.png");
	ui_textures[ADVANCE]	= (Texture*)App->importer->ImportTexturePointer("Editor textures/Advance.png");

	ui_textures[GUIZMO_TRANSLATE]	= (Texture*)App->importer->ImportTexturePointer("Editor textures/translate.png");
	ui_textures[GUIZMO_ROTATE]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/rotate.png");
	ui_textures[GUIZMO_SCALE]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/scale.png");
	ui_textures[GUIZMO_LOCAL]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/Guizmo_local.png");
	ui_textures[GUIZMO_GLOBAL]		= (Texture*)App->importer->ImportTexturePointer("Editor textures/Guizmo_global.png");
	ui_textures[GUIZMO_SELECT] = (Texture*)App->importer->ImportTexturePointer("Editor textures/Guizmo_select.png");

	ui_textures[NO_TEXTURE] = (Texture*)App->importer->ImportTexturePointer("Editor textures/no_texture.png");

	ui_textures[FOLDER_ICON] = (Texture*)App->importer->ImportTexturePointer("Editor textures/folder_icon.png");
	ui_textures[OBJECT_ICON] = (Texture*)App->importer->ImportTexturePointer("Editor textures/object_icon.png");
	ui_textures[SCENE_ICON] = (Texture*)App->importer->ImportTexturePointer("Editor textures/scene_icon.png");
	ui_textures[RETURN_ICON] = (Texture*)App->importer->ImportTexturePointer("Editor textures/return_icon.png");


	ui_fonts[TITLES]				= io->Fonts->AddFontFromFileTTF("Fonts/title.ttf", 16.0f);
	ui_fonts[REGULAR]				= io->Fonts->AddFontFromFileTTF("Fonts/regular.ttf", 18.0f);
	//ui_fonts[REGULAR_BOLD]		= io->Fonts->AddFontFromFileTTF("Fonts/regular_bold.ttf", 18.0f);
	//ui_fonts[REGULAR_ITALIC]		= io->Fonts->AddFontFromFileTTF("Fonts/regular_italic.ttf", 18.0f);
	//ui_fonts[REGULAR_BOLDITALIC]	= io->Fonts->AddFontFromFileTTF("Fonts/regular_bold_italic.ttf", 18.0f);
	
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io->IniFilename = "Settings\\imgui.ini";
	docking_background = true;

	return true;
}

update_status ModuleUI::PreUpdate(float dt) {

	// Start the ImGui frame
	ImGui_ImplOpenGL2_NewFrame();

	ImGui_ImplSDL2_NewFrame(App->window->main_window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleUI::Update(float dt) {


	InvisibleDockingBegin();
	static bool file_save = false;

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) 
		open_tabs[CONFIGURATION] = !open_tabs[CONFIGURATION];

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		open_tabs[VIEWPORT_MENU] = !open_tabs[VIEWPORT_MENU];
		for (int i = 0; i < 6; i++)
			App->camera->viewports[i]->active = open_tabs[VIEWPORT_MENU];
	}

	if (open_tabs[CONFIGURATION]) 
	{
		ImGui::Begin("Configuration", &open_tabs[CONFIGURATION]);

		if (ImGui::CollapsingHeader("Graphics")) 
			DrawGraphicsLeaf();
		if (ImGui::CollapsingHeader("Window"))
			DrawWindowConfigLeaf();
		if (ImGui::CollapsingHeader("Hardware"))
			DrawHardwareLeaf();
		if (ImGui::CollapsingHeader("Application"))
			DrawApplicationLeaf();

		if (ImGui::Button("Reset Camera"))
			App->camera->editor_camera->Reset();

		ImGui::End();
	}

	if (open_tabs[HIERARCHY])
		DrawHierarchyTab();

	Camera* prev_selected = App->camera->background_camera;
	App->camera->selected_camera = nullptr;

	if (open_tabs[OBJ_INSPECTOR])
		DrawObjectInspectorTab();

	if (!App->camera->selected_camera)
		App->camera->selected_camera = prev_selected;

	if (open_tabs[PRIMITIVE])
		DrawPrimitivesTab();

	if (open_tabs[ABOUT])
		DrawAboutLeaf();
	
	if (open_tabs[LOG])
		app_log->Draw("App log",&open_tabs[LOG]);

	if (open_tabs[TIME_CONTROL])
		DrawTimeControlWindow();

	if (open_tabs[QUADTREE_CONFIG])
		DrawQuadtreeConfigWindow();

	if (open_tabs[CAMERA_MENU])
		DrawCameraMenuWindow();

	if (open_tabs[VIEWPORT_MENU])
		DrawViewportsWindow();

	if (open_tabs[ASSET_WINDOW])
		DrawAssetsWindow();

	if (open_tabs[RESOURCES_TAB]) {
		DrawResourcesWindow();
	}
/*
	if (open_tabs[AUDIO])
		DrawAudioTab();*/

	if (App->scene->selected_obj && !App->scene->selected_obj->is_static) // Not draw guizmo if it is static
		App->gui->DrawGuizmo();

	for (auto it = App->camera->game_cameras.begin(); it != App->camera->game_cameras.end(); it++)
	{
		if ((*it)->draw_in_UI)
			DrawCameraViewWindow(*(*it));
	}

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit"))
				App->CLOSE_APP();
			if (ImGui::MenuItem("Import file"))
			{
				std::string file_path = openFileWID();
				App->resources->LoadFileToScene(file_path.c_str());
			}
			if (ImGui::MenuItem("Save scene")) {
				if (App->scene->existingScene())
					App->scene->AskSceneSaveFile((char*)App->scene->getWorkigSceneName().c_str());
				else
					file_save = true;
				
			}
			if (ImGui::MenuItem("Save scene as..."))
				file_save = true;
			if (ImGui::MenuItem("Load scene")){
				std::string file_path = openFileWID();
				App->scene->AskSceneLoadFile((char*)file_path.c_str());
			}
			if(ImGui::BeginMenu("Configuration")){
				if (ImGui::MenuItem("Save Configuration"))
					App->SaveConfig();
				if (ImGui::MenuItem("Delete Configuration"))
					App->DeleteConfig();
				if (ImGui::MenuItem("Load Default Configuration"))
					App->LoadDefaultConfig();
				ImGui::EndMenu();
			}

			if (ImGui::MenuItem("Clean library (App will shut down)"))
				App->resources->CleanResources();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Hierarchy", NULL, &open_tabs[HIERARCHY]);
			ImGui::MenuItem("Object Inspector", NULL, &open_tabs[OBJ_INSPECTOR]);
			ImGui::MenuItem("Primitive", NULL, &open_tabs[PRIMITIVE]);
			ImGui::MenuItem("Configuration", NULL, &open_tabs[CONFIGURATION]);
			ImGui::MenuItem("Log", NULL, &open_tabs[LOG]);
			ImGui::MenuItem("Time control", NULL, &open_tabs[TIME_CONTROL]);
			ImGui::MenuItem("Quadtree", NULL, &open_tabs[QUADTREE_CONFIG]);
			ImGui::MenuItem("Camera Menu", NULL, &open_tabs[CAMERA_MENU]);
			ImGui::MenuItem("Asset Window", NULL, &open_tabs[ASSET_WINDOW]);
			ImGui::MenuItem("Resources Window", NULL, &open_tabs[RESOURCES_TAB]);
			//ImGui::MenuItem("Audio", NULL, &open_tabs[AUDIO]);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help")) {
			ImGui::MenuItem("About", NULL, &open_tabs[ABOUT]);
			if(ImGui::MenuItem("Documentation"))
				App->requestBrowser("https://github.com/Skyway666/Kuroko-Engine/wiki");
			if(ImGui::MenuItem("Download latest"))
				App->requestBrowser("https://github.com/Skyway666/Kuroko-Engine/releases");
			if(ImGui::MenuItem("Report a bug"))
				App->requestBrowser("https://github.com/Skyway666/Kuroko-Engine/issues");
			ImGui::EndMenu();
		}

		std::string current_viewport = "Viewport: Free camera";

		switch (App->camera->selected_camera->getViewportDir())
		{
		case VP_RIGHT: current_viewport = "Viewport: Right"; break;
		case VP_LEFT: current_viewport	= "Viewport: Left"; break;
		case VP_UP: current_viewport	= "Viewport: Up"; break;
		case VP_DOWN: current_viewport	= "Viewport: Down"; break;
		case VP_FRONT: current_viewport = "Viewport: Front"; break;
		case VP_BACK: current_viewport	= "Viewport: Back"; break;
		default: break;
		}
			
		if (ImGui::BeginMenu(current_viewport.c_str())) 
		{
			if (App->camera->selected_camera != App->camera->editor_camera)
			{
				if (ImGui::MenuItem("Free Camera"))
				{
					App->camera->selected_camera->active = false;
					App->camera->selected_camera = App->camera->background_camera = App->camera->editor_camera;
					App->camera->selected_camera->active = true;
				}
			}

			std::string viewport_name;
			for (int i = 0; i < 6; i++)
			{
				if (App->camera->selected_camera != App->camera->viewports[i])
				{
					if (ImGui::MenuItem(App->camera->viewports[i]->getViewportDirString().c_str()))
					{
						App->camera->background_camera->active = false;
						App->camera->background_camera = App->camera->selected_camera = App->camera->viewports[i];
						App->camera->background_camera->active = true;
					}
				}
			}

			ImGui::MenuItem("Open viewport menu", nullptr, open_tabs[VIEWPORT_MENU]);
			
			ImGui::EndMenu();
		}
	
	}
	ImGui::EndMainMenuBar();

	if (file_save) {
		ImGui::Begin("Scene Name");
		ImGui::PushFont(ui_fonts[REGULAR]);

		static char rename_buffer[64];
		ImGui::InputText("Save as...", rename_buffer, 64);
		ImGui::SameLine();
		if (ImGui::Button("Save")) {
			App->scene->AskSceneSaveFile(rename_buffer);
			for (int i = 0; i < 64; i++)
				rename_buffer[i] = '\0';
			file_save = false;
		}
		ImGui::PopFont();
		ImGui::End();
	}
	InvisibleDockingEnd();
	return UPDATE_CONTINUE;

}

update_status ModuleUI::PostUpdate(float dt) {
	// Rendering
	ImGui::Render();
	
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;

}

bool ModuleUI::CleanUp() 
{	
	for (int i = 0; i < LAST_UI_TEX; i++) { // Cleanup textures
		delete ui_textures[i];
	}
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}




void ModuleUI::DrawHierarchyTab()
{
	ImGui::Begin("Hierarchy Tab", &open_tabs[HIERARCHY]);
	ImGui::PushFont(ui_fonts[REGULAR]);

	if (ImGui::Button("Empty gameobject")) {
		GameObject* go = new GameObject("Empty", App->scene->selected_obj);
		if(App->scene->selected_obj)
			App->scene->selected_obj->addChild(go);
	}

	ImGui::SameLine();

	if (ImGui::Button("Duplicate Selected")) {
		if (!App->scene->duplicateGameObject(App->scene->selected_obj)) {
			app_log->AddLog("Nothing is selected, can't duplicate");
		}
	}

	int id = 0;
	std::list<GameObject*> root_objs;
	App->scene->getRootObjs(root_objs);

	for (auto it = root_objs.begin(); it != root_objs.end(); it++){
		DrawHierarchyNode(*(*it), id);
		id++;
	}

	if (ImGui::IsWindowHovered())
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			App->scene->selected_obj = nullptr;
	}

	ImGui::PopFont();
	ImGui::End();
}

void ModuleUI::DrawHierarchyNode(const GameObject& game_object, int& id) const
{
	id++;
	static int selection_mask = (1 << 2);
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << id)) ? ImGuiTreeNodeFlags_Selected : 0);

	std::list<GameObject*> children;
	game_object.getChildren(children);

	if(children.empty())
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; 

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, game_object.getName().c_str(), id) && !children.empty();

	if(App->scene->selected_obj == (GameObject*)&game_object)
		selection_mask = (1 << id);
	else if (App->scene->selected_obj == nullptr)
		selection_mask = (1 >> id);

	if (ImGui::IsItemClicked())
		App->scene->selected_obj = (GameObject*)&game_object;

	if (node_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);

		for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
				DrawHierarchyNode(*(*it), id);

		ImGui::PopStyleVar();
		ImGui::TreePop();
	}
}

void ModuleUI::DrawObjectInspectorTab()
{
	ImGui::Begin("Object inspector", &open_tabs[OBJ_INSPECTOR]);
	ImGui::PushFont(ui_fonts[REGULAR]);

	static bool show_rename = false;
	GameObject* selected_obj = App->scene->selected_obj;

	if (selected_obj)
	{
		ImGui::Text("Name: %s", selected_obj->getName().c_str());

		ImGui::SameLine();
		if (ImGui::Button("Rename"))
			show_rename = true;

		ImGui::SameLine();
		if (ImGui::Button("Delete") || App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
			App->scene->deleteGameObjectRecursive(selected_obj);


		ImGui::Checkbox("Active", &selected_obj->is_active);
		ImGui::SameLine();
		if (ImGui::Checkbox("Static", &selected_obj->is_static)) // If an object is set/unset static, reload the quadtree
			App->scene->quadtree_reload = true;
		

		if (ImGui::CollapsingHeader("Add component"))
		{
			if (ImGui::Button("Add Mesh"))	selected_obj->addComponent(MESH); // MAKES SKYBOX DIE
			if (ImGui::Button("Add Camera"))  selected_obj->addComponent(CAMERA);
		}

		std::list<Component*> components;
		selected_obj->getComponents(components);

		std::list<Component*> components_to_erase;
		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++)
			if (!DrawComponent(*(*it)))
				components_to_erase.push_back(*it);

		for (std::list<Component*>::iterator it = components_to_erase.begin(); it != components_to_erase.end(); it++)
			selected_obj->removeComponent(*it);
	}
	else if (show_rename)
		show_rename = false;

	ImGui::PopFont();
	ImGui::End();
	
	if (show_rename)
	{
		ImGui::SetNextWindowPos(ImVec2(700, 320), ImGuiCond_FirstUseEver); 
		ImGui::Begin("Rename object");
		ImGui::PushFont(ui_fonts[REGULAR]);

		static char rename_buffer[64];
		ImGui::InputText("##Rename to", rename_buffer, 64);

		ImGui::SameLine();
		if (ImGui::Button("OK##Change name"))
		{
			selected_obj->Rename(rename_buffer);
			show_rename = false;
		}

		ImGui::PopFont();
		ImGui::End();
	}
}

bool ModuleUI::DrawComponent(Component& component)
{
	ComponentCamera* camera = nullptr; // aux pointer
	switch (component.getType())
	{
	case MESH:
		if (ImGui::CollapsingHeader("Mesh"))
		{
			ComponentMesh* c_mesh = (ComponentMesh*)&component;
			static bool wireframe_enabled;
			static bool mesh_active;
			static bool draw_normals;

			wireframe_enabled = c_mesh->getWireframe();
			draw_normals = c_mesh->getDrawNormals();
			mesh_active = c_mesh->isActive();

			if (ImGui::Checkbox("Active## mesh_active", &mesh_active))
				c_mesh->setActive(mesh_active);

			if (mesh_active)
			{

				if (ImGui::Checkbox("Wireframe", &wireframe_enabled))
					c_mesh->setWireframe(wireframe_enabled);
				ImGui::SameLine();
				if (ImGui::Checkbox("Draw normals", &draw_normals))
					c_mesh->setDrawNormals(draw_normals);
				
				
				if (Material* material = c_mesh->getMaterial())
				{
					if (ImGui::TreeNode("Material"))
					{
						static int preview_size = 128;
						ImGui::Text("Id: %d", material->getId());
						ImGui::SameLine();
						if (ImGui::Button("remove material"))
						{
							c_mesh->setMaterial(nullptr);
							ImGui::TreePop();
							return true;
						}

						ImGui::Text("Preview size");
						ImGui::SameLine();
						if (ImGui::Button("64")) preview_size = 64;
						ImGui::SameLine();
						if (ImGui::Button("128")) preview_size = 128;
						ImGui::SameLine();
						if (ImGui::Button("256")) preview_size = 256;

						

						if(ImGui::TreeNode("diffuse"))
						{
							Texture* texture = nullptr;
							if(ResourceTexture* tex_res = (ResourceTexture*)App->resources->getResource(material->getTextureResource(DIFFUSE)))
								texture = tex_res->texture;


							ImGui::Image(texture ? (void*)texture->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));
							ImGui::SameLine();

							int w = 0; int h = 0;
							if(texture)
								texture->getSize(w, h);

							ImGui::Text("texture data: \n x: %d\n y: %d", w, h);

							//if (ImGui::Button("Load checkered##Dif: Load checkered"))
							//	material->setCheckeredTexture(DIFFUSE);
							//ImGui::SameLine()
							if (ImGui::Button("Load##Dif: Load"))
							{
								std::string texture_path = openFileWID();
								uint new_resource = App->resources->getResourceUuid(texture_path.c_str());
								if(new_resource != 0){
									App->resources->assignResource(new_resource);
									App->resources->deasignResource(material->getTextureResource(DIFFUSE));
									material->setTextureResource(DIFFUSE, new_resource);
								}
							}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("ambient (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(AMBIENT) ? (void*)material->getTexture(AMBIENT)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Amb: Load checkered"))
							//	material->setCheckeredTexture(AMBIENT);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Amb: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(AMBIENT, tex);
							//}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("normals (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(NORMALS) ? (void*)material->getTexture(NORMALS)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Nor: Load checkered"))
							//	material->setCheckeredTexture(NORMALS);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Nor: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(NORMALS, tex);
							//}
							ImGui::TreePop();
						}

						if (ImGui::TreeNode("lightmap (feature not avaliable yet)"))
						{
							//ImGui::Image(material->getTexture(LIGHTMAP) ? (void*)material->getTexture(LIGHTMAP)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

							//if (ImGui::Button("Load checkered##Lgm: Load checkered"))
							//	material->setCheckeredTexture(LIGHTMAP);
							//ImGui::SameLine();
							//if (ImGui::Button("Load##Lgm: Load"))
							//{
							//	std::string texture_path = openFileWID();
							//	if (Texture* tex = (Texture*)App->importer->Import(texture_path.c_str(), I_TEXTURE))
							//		c_mesh->getMaterial()->setTexture(LIGHTMAP, tex);
							//}
							//ImGui::TreePop();
						}
						ImGui::TreePop();
					}
				}
				else
				{
					if (ImGui::Button("Add material"))
					{
						Material* mat = new Material();
						c_mesh->setMaterial(mat);
					}
				}

				if (Mesh* mesh = c_mesh->getMesh())
				{
					if (ImGui::TreeNode("Mesh Data"))
					{
						uint vert_num, poly_count;
						bool has_normals, has_colors, has_texcoords;

						mesh->getData(vert_num, poly_count, has_normals, has_colors, has_texcoords);
						ImGui::Text("vertices: %d, poly count: %d, ", vert_num, poly_count);
						ImGui::Text(has_normals ? "normals: Yes," : "normals: No,");
						ImGui::Text(has_colors ? "colors: Yes," : "colors: No,");
						ImGui::Text(has_texcoords ? "tex coords: Yes" : "tex coords: No");

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Remove##Remove mesh"))
				return false;
		}
		break;
	case TRANSFORM:
		if (ImGui::CollapsingHeader("Transform")) 
		{
			ImGui::TextWrapped("Drag the parameters to change them, or ctrl+click on one of them to set it's value");
			ComponentTransform* c_trans = (ComponentTransform*)&component;

			static float3 position;
			static float3 rotation;
			static float3 scale;

			Transform* transform = nullptr;

			if (c_trans->getMode() == LOCAL)
			{
				transform = c_trans->local;
				ImGui::Text("Current mode: Local");
				ImGui::SameLine();
				if (ImGui::Button("Global"))
					c_trans->setMode(GLOBAL);
			}
			else
			{
				transform = c_trans->global;
				ImGui::Text("Current mode: Global");
				ImGui::SameLine();
				if (ImGui::Button("Local"))
					c_trans->setMode(LOCAL);
			}

			position = transform->getPosition();
			rotation = transform->getRotationEuler();
			scale = transform->getScale();

			//position
			ImGui::Text("Position:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if(!c_trans->constraints[0][0])		ImGui::DragFloat("##p x", &position.x, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[0][1])	ImGui::DragFloat("##p y", &position.y, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.y);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[0][2])	ImGui::DragFloat("##p z", &position.z, 0.01f, 0.0f, 0.0f, "%.02f");
			else								ImGui::Text("%.2f", position.z);
			
			//rotation
			ImGui::Text("Rotation:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][0])	ImGui::DragFloat("##r x", &rotation.x, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.z);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][1])	ImGui::DragFloat("##r y", &rotation.y, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[1][2])	ImGui::DragFloat("##r z", &rotation.z, 0.2f, -180.0f, 180.0f, "%.02f");
			else								ImGui::Text("%.2f", rotation.y);

			//scale
			ImGui::Text("   Scale:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][0])	ImGui::DragFloat("##s x", &scale.x, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.x);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][1])	ImGui::DragFloat("##s y", &scale.y, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.y);

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			if (!c_trans->constraints[2][2])	ImGui::DragFloat("##s z", &scale.z, 0.01f, -1000.0f, 1000.0f, "%.02f");
			else								ImGui::Text("%.2f", scale.z);

			if (ImGui::Button("Reset Transform"))
			{
				position = float3::zero; rotation = float3::zero, scale = float3::one;
			}

			ImGui::Checkbox("Draw axis", &c_trans->draw_axis);
			
			if (ImGui::CollapsingHeader("	Constraints"))
			{
				ImGui::Text("Position:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p x", &c_trans->constraints[0][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p y", &c_trans->constraints[0][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint p z", &c_trans->constraints[0][2]);


				ImGui::Text("Rotation:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r x", &c_trans->constraints[1][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r y", &c_trans->constraints[1][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint r z", &c_trans->constraints[1][2]);


				ImGui::Text("   Scale:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s x", &c_trans->constraints[2][0]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s y", &c_trans->constraints[2][1]);

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::Checkbox("##constraint s z", &c_trans->constraints[2][2]);
			}

			transform->setPosition(position);
			transform->setRotationEuler(rotation);
			transform->setScale(scale);
		}
		break;

	case C_AABB:
		if (ImGui::CollapsingHeader("AABB"))
		{
			ComponentAABB* aabb = (ComponentAABB*)&component;

			static bool aabb_active;
			aabb_active = aabb->isActive();

			if (ImGui::Checkbox("Active##active AABB", &aabb_active))
				aabb->setActive(aabb_active);

			if (aabb_active)
			{
				static bool aabb_drawn;
				aabb_drawn = aabb->draw_aabb;

				if (ImGui::Checkbox("draw AABB", &aabb_drawn))
					aabb->draw_aabb = aabb_drawn;

				static bool obb_drawn;
				obb_drawn = aabb->draw_obb;

				ImGui::SameLine();
				if (ImGui::Checkbox("draw OBB", &obb_drawn))
					aabb->draw_obb = obb_drawn;

				if (ImGui::Button("Reload##Reload AABB"))
					aabb->Reload();
			}

		}
		break;
	case CAMERA:

		camera = (ComponentCamera*)&component;

		if (ImGui::CollapsingHeader("Camera"))
		{
			static bool camera_active;
			camera_active = camera->isActive();

			if (ImGui::Checkbox("Active##active camera", &camera_active))
				camera->setActive(camera_active);

			ImGui::Checkbox("Draw camera view", &camera->getCamera()->draw_in_UI);

			ImGui::Checkbox("Draw frustum", &camera->getCamera()->draw_frustum);

			ImGui::Checkbox("Draw depth", &camera->getCamera()->draw_depth);

			static bool overriding;
			overriding = (camera->getCamera() == App->camera->override_editor_cam_culling);
			if (ImGui::Checkbox("Override Frustum Culling", &overriding))
			{
				if (!overriding)	App->camera->override_editor_cam_culling = nullptr;
				else				App->camera->override_editor_cam_culling = camera->getCamera();
			}

			if (camera_active)
			{
				static float3 offset;
				offset = camera->offset;
				ImGui::Text("Offset:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o x", &offset.x, 0.01f, -1000.0f, 1000.0f, "%.02f");

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o y", &offset.y, 0.01f, -1000.0f, 1000.0f, "%.02f");

				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##o z", &offset.z, 0.01f, -1000.0f, 1000.0f, "%.02f");

				camera->offset = offset;
			}

			if (ImGui::Button("Remove##Remove camera"))
				return false;
		}
	default:
		break;
	}

	return true;
}

void ModuleUI::DrawCameraViewWindow(Camera& camera)
{
	if (FrameBuffer* frame_buffer = camera.getFrameBuffer())
	{
		std::string window_name;

		if (camera.getParent())
			window_name = camera.getParent()->getParent()->getName() + " Camera";
		else
			window_name = camera.getViewportDirString();

		ImGui::Begin(window_name.c_str(), &camera.draw_in_UI, ImGuiWindowFlags_NoResize);

		if(ImGui::IsWindowFocused())
			App->camera->selected_camera = &camera;
		
		ImGui::SetWindowSize(ImVec2(frame_buffer->size_x / 3 + 50, frame_buffer->size_y / 3 + 70));

		if (ImGui::ImageButton((void*) (camera.draw_depth ? frame_buffer->depth_tex->gl_id : frame_buffer->tex->gl_id), ImVec2(frame_buffer->size_x / 3, frame_buffer->size_y / 3), nullptr, ImVec2(0, 1), ImVec2(1, 0)))
		{
			float x = App->input->GetMouseX(); float y = App->input->GetMouseY();
			ImVec2 window_pos = ImGui::GetWindowPos();
			x = (((x - window_pos.x) / ImGui::GetWindowSize().x) * 2) - 1;
			y = (((y - window_pos.y) / ImGui::GetWindowSize().y) * 2) - 1;

			if (GameObject* new_selected = App->scene->MousePicking(x, y, camera.getParent() ? camera.getParent()->getParent() : nullptr))
				App->scene->selected_obj = new_selected;
		}
		ImGui::End();
	}
	else camera.initFrameBuffer();
}

void ModuleUI::DrawViewportsWindow()
{
	ImGui::Begin("Viewports", nullptr);

	if (ImGui::Button("Close##Close viewports"))
	{
		for (int i = 0; i < 6; i++)
			App->camera->viewports[i]->active = false;

		open_tabs[VIEWPORT_MENU] = false;
	}

	for (int i = 0; i < 6; i++)
	{
		if (i != 0 && i != 3)
			ImGui::SameLine();

		FrameBuffer* fb = App->camera->viewports[i]->getFrameBuffer();
		ImGui::TextWrapped(App->camera->viewports[i]->getViewportDirString().c_str());
		ImGui::SameLine();

		if (ImGui::ImageButton((void*)(App->camera->viewports[i]->draw_depth ? fb->depth_tex->gl_id : fb->tex->gl_id), ImVec2(fb->size_x / 4, fb->size_y / 4), nullptr, ImVec2(0, 1), ImVec2(1, 0)))
		{
			for (int i = 0; i < 6; i++)
				App->camera->viewports[i]->active = false;

			App->camera->background_camera->active = false;
			App->camera->background_camera = App->camera->selected_camera = App->camera->viewports[i];
			App->camera->background_camera->active = true;
			open_tabs[VIEWPORT_MENU] = false;
		}
	}
	ImGui::End();
}

void ModuleUI::DrawCameraMenuWindow()
{
	ImGui::Begin("Camera Menu", &open_tabs[CAMERA_MENU]);

	if (App->camera->override_editor_cam_culling)
	{
		ImGui::TextWrapped("Background camera frustum culling overriden by camera %s", App->camera->override_editor_cam_culling->getParent()->getParent()->getName().c_str());
		if (ImGui::Button("Stop overriding"))
			App->camera->override_editor_cam_culling = nullptr;
	}

	static bool hide_viewports = false;

	ImGui::Checkbox("Hide viewports", &hide_viewports);

	for (auto it = App->camera->game_cameras.begin(); it != App->camera->game_cameras.end(); it++)
	{
		if ((*it)->IsViewport() && hide_viewports)
			continue;

		std::string name;
		if ((*it)->getParent()) 
			name = (*it)->getParent()->getParent()->getName() + "Camera";
		else
		{
			if ((*it) == App->camera->editor_camera)
				name = "Free camera";
			else
				name = (*it)->getViewportDirString();
		}

		if (ImGui::TreeNode(name.c_str()))
		{
			if ((*it)->active)  ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Active");
			else				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unactive");

			ImGui::SameLine();
			if (*it != App->camera->background_camera)
			{
				static bool is_overriding;
				is_overriding = ((*it) == App->camera->override_editor_cam_culling);
				if (ImGui::Checkbox("Override Frustum Culling", &is_overriding))
				{
					if (!is_overriding)  App->camera->override_editor_cam_culling = nullptr;
					else				 App->camera->override_editor_cam_culling = *it;
				}
			}
			else
				ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Background camera");
			
			ImGui::Checkbox("Draw camera view", &(*it)->draw_in_UI);

			ImGui::Checkbox("Draw frustum", &(*it)->draw_frustum);

			ImGui::Checkbox("Draw depth", &(*it)->draw_depth);

			if(ImGui::CollapsingHeader("Frustum"))
			{
				if ((*it)->getFrustum()->type == math::FrustumType::PerspectiveFrustum)
				{
					ImGui::Text("Current mode: Perspective");
					ImGui::SameLine();
					if (ImGui::Button("Set ortographic"))
					{
						(*it)->getFrustum()->type = math::FrustumType::OrthographicFrustum;
						(*it)->getFrustum()->orthographicHeight = (*it)->getFrustum()->orthographicWidth = INIT_ORT_SIZE;
					}

					static float hor_fov = RADTODEG * (*it)->getFrustum()->horizontalFov;
					static float ver_fov = RADTODEG * (*it)->getFrustum()->verticalFov;

					ImGui::Text("Horizontal FOV:");
					ImGui::SameLine();
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
					if (ImGui::DragFloat("##Horizontal FOV", &hor_fov, 1.0f, 1.0f, 179.0f, "%.02f"))
						(*it)->getFrustum()->horizontalFov = DEGTORAD * hor_fov;

					ImGui::Text("Vertical FOV:");
					ImGui::SameLine();
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
					if (ImGui::DragFloat("##Vertical FOV", &ver_fov, 1.0f, 1.0f, 179.0f, "%.02f"))
						(*it)->getFrustum()->verticalFov = DEGTORAD * ver_fov;

				}
				else
				{

					ImGui::Text("Current mode: Ortographic");
					ImGui::SameLine();

					if (ImGui::Button("Set perspective"))
					{
						(*it)->getFrustum()->type = math::FrustumType::PerspectiveFrustum;
						(*it)->getFrustum()->horizontalFov = DEGTORAD * INIT_HOR_FOV; (*it)->getFrustum()->verticalFov = DEGTORAD * INIT_VER_FOV;
					}

					ImGui::Text("Ortographic Width:");
					ImGui::SameLine();
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##Ortographic Width", &(*it)->getFrustum()->orthographicWidth, 1.0f, 1.0f, 500.0f, "%.02f");

					ImGui::Text("Ortographic Height:");
					ImGui::SameLine();
					ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
					ImGui::DragFloat("##Ortographic Height", &(*it)->getFrustum()->orthographicHeight, 1.0f, 1.0f, 500.0f, "%.02f");

				}

				ImGui::Text("Near Plane:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##Near Plane:", &(*it)->getFrustum()->nearPlaneDistance, 0.1f, 0.0f, 500.0f, "%.02f");

				ImGui::Text("Far Plane:");
				ImGui::SameLine();
				ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
				ImGui::DragFloat("##Far Plane:", &(*it)->getFrustum()->farPlaneDistance, 0.1f, 1.0f, 1500.0f, "%.02f");

			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void ModuleUI::DrawAssetsWindow()
{
	ImGui::Begin("Assets Window", &open_tabs[ASSET_WINDOW]);
	int element_size = 64;
	std::string path, name, extension;
	
	int column_num = (int)trunc(ImGui::GetWindowSize().x / (element_size + 20));

	if (column_num != 0)
	{
		int count = 0;
		int iteration = 0;

		if (ImGui::ImageButton((void*)ui_textures[RETURN_ICON]->getGLid(), ImVec2(24, 17)))
		{
			if (!App->fs.getPath(asset_window_path))
				asset_window_path = ASSETS_FOLDER;
		}

		ImGui::SameLine();
		ImGui::Text(asset_window_path.c_str());

		using std::experimental::filesystem::directory_iterator;
		for (auto& it : directory_iterator(asset_window_path))
		{
			extension = it.path().generic_string();
			App->fs.getExtension(extension);
			if(extension != ".meta")
				count++;
		}

		if (count < column_num) column_num = count;
		count = 0;

		ImGui::Columns(column_num, (std::to_string(iteration) + " asset columns").c_str(), false);

		for (auto& it : directory_iterator(asset_window_path)) {

			path = name = extension = it.path().generic_string();	// Separate path, name and extension	
			App->fs.getExtension(extension);
			App->fs.getPath(path);
			App->fs.getFileNameFromPath(name);

			if (extension == ".meta")
				continue;

			if (count == column_num)
			{
				ImGui::NewLine();
				iteration++;
				count = 0;
				ImGui::Columns(column_num, (std::to_string(iteration) + " asset columns").c_str(), false);
			}
			count++;

			if(column_num > 1)
				ImGui::SetColumnWidth(ImGui::GetColumnIndex(), element_size + 20);

			if (it.status().type() == std::experimental::filesystem::v1::file_type::directory)
			{

				if (ImGui::IsMouseDoubleClicked(0))
				{
					ImGui::ImageButton((void*)ui_textures[FOLDER_ICON]->getGLid(), ImVec2(element_size, element_size), it.path().generic_string().c_str(), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, selected_asset == it.path().generic_string() ? 1.0f : 0.0f));
					if(ImGui::IsItemHovered())
						asset_window_path = it.path().generic_string();
				}
				else {
					if (ImGui::ImageButton((void*)ui_textures[FOLDER_ICON]->getGLid(), ImVec2(element_size, element_size), it.path().generic_string().c_str(), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, selected_asset == it.path().generic_string() ? 1.0f : 0.0f)))
						selected_asset = it.path().generic_string();
				}
			}
			else
			{
				const char* type = App->resources->assetExtension2type(extension.c_str());

				if (type == "scene")
				{
					if (ImGui::ImageButton((void*)ui_textures[OBJECT_ICON]->getGLid(), ImVec2(element_size, element_size), it.path().generic_string().c_str(), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, selected_asset == it.path().generic_string() ? 1.0f : 0.0f)))
						selected_asset = it.path().generic_string();
				}
				if (type == "texture")
				{
					ResourceTexture* res_tex = (ResourceTexture*)App->resources->getResource(App->resources->getResourceUuid(it.path().generic_string().c_str()));
					
					res_tex->drawn_in_UI = true;
					if (!res_tex->IsLoaded())
						res_tex->LoadToMemory();

					if (ImGui::ImageButton((void*)res_tex->texture->getGLid(), ImVec2(element_size, element_size), it.path().generic_string().c_str(), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, selected_asset == it.path().generic_string() ? 1.0f : 0.0f)))
						selected_asset = it.path().generic_string();
				}
				if (type == "json")
				{
					if (ImGui::ImageButton((void*)ui_textures[SCENE_ICON]->getGLid(), ImVec2(element_size, element_size), it.path().generic_string().c_str(), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, selected_asset == it.path().generic_string() ? 1.0f : 0.0f)))
						selected_asset = it.path().generic_string();
				}
			}
			
			ImGui::TextWrapped(name.c_str());
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
	ImGui::End();

	DrawAssetInspector();
}

void ModuleUI::DrawAssetInspector()
{
	ImGui::Begin("Asset inspector", nullptr);

	if (!selected_asset.empty())
	{
		std::string name, extension;
		extension = name = selected_asset;
		App->fs.getExtension(extension);
		App->fs.getFileNameFromPath(name);

		ImGui::Text(name.c_str());
		
		if (extension.empty())  // is directory
		{
			ImGui::Text("type: directory");
			ImGui::End();
			return;
		}
		else if (extension == ".json")
		{
			ImGui::Text("type: scene");
			ImGui::End();
			return;
		}

		const char* type = App->resources->assetExtension2type(extension.c_str());
		if(type == "scene")
			ImGui::Text("type: 3D object");
		else
			ImGui::Text("type: %s", &type);

		Resource* res = App->resources->getResource(App->resources->getResourceUuid(selected_asset.c_str()));
		ImGui::Text("Used by %s components", std::to_string(res->components_used_by).c_str());

		if(res->IsLoaded())		ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Loaded");
		else					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Unloaded");

		if (type == "texture")
		{
			ResourceTexture* res_tex = (ResourceTexture*)res;

			res_tex->drawn_in_UI = true;
			if (!res_tex->IsLoaded())
				res_tex->LoadToMemory();

			int size_x, size_y;
			res_tex->texture->getSize(size_x, size_y);
			ImGui::Image((void*)res_tex->texture->getGLid(), ImVec2((float)size_x, (float)size_y));

			ImGui::Scrollbar(ImGuiLayoutType_::ImGuiLayoutType_Horizontal);
		}
	}

	ImGui::End();
}

void ModuleUI::DrawResourcesWindow()
{
	ImGui::Begin("Resources Window", &open_tabs[RESOURCES_TAB]);
	static float refresh_ratio = 1;
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
	if (ImGui::InputFloat("Refresh ratio (seconds)", &refresh_ratio)) {
		App->resources->setRefreshRatio(refresh_ratio * 1000);
	}
	ImGui::End();
}

//void ModuleUI::DrawAudioTab()
//{
//	ImGui::Begin("Audio", &open_tabs[AUDIO]);
//	ImGui::PushFont(ui_fonts[REGULAR]);
//	ImGui::Text("Use this tab to check and play loaded audio files");
//	
//	for (auto it = App->audio->audio_files.begin(); it != App->audio->audio_files.end(); it++)
//	{
//		if (ImGui::TreeNode((*it)->name.c_str()))
//		{
//			ImGui::Text("type: %s", (*it)->type == FX ? "FX" : "Music");
//			ImGui::SameLine();
//			if (ImGui::ImageButton((void*)ui_textures[PLAY]->getGLid(), ImVec2(16, 16)))
//				(*it)->Play();
//			ImGui::SameLine();
//			if (ImGui::ImageButton((void*)ui_textures[STOP]->getGLid(), ImVec2(16, 16)))
//				(*it)->Stop();
//			ImGui::TreePop();
//		}
//	}
//
//  ImGui::PopFont();
//	ImGui::End();
//}

void ModuleUI::DrawPrimitivesTab() 
{
	ImGui::Begin("Primitives", &open_tabs[PRIMITIVE]);
	ImGui::PushFont(ui_fonts[REGULAR]);

	if (ImGui::Button("Add cube")){
		GameObject* cube = new GameObject("Cube");
		cube->addComponent(C_AABB);
		cube->addComponent(new ComponentMesh(cube, Primitive_Cube));
	}
	ImGui::SameLine();
	if (ImGui::Button("Add plane")){
		GameObject* plane = new GameObject("Plane");
		plane->addComponent(C_AABB);
		plane->addComponent(new ComponentMesh(plane, Primitive_Plane));
	}
	if (ImGui::Button("Add sphere")) {
		GameObject* sphere = new GameObject("Sphere");
		sphere->addComponent(C_AABB);
		sphere->addComponent(new ComponentMesh(sphere, Primitive_Sphere));
	}
	ImGui::SameLine();
	if (ImGui::Button("Add cylinder")) {
		GameObject* cylinder = new GameObject("Cylinder");
		cylinder->addComponent(C_AABB);
		cylinder->addComponent(new ComponentMesh(cylinder, Primitive_Cylinder));
	}

	ImGui::PopFont();
	ImGui::End();
}


void ModuleUI::DrawAboutLeaf()
{
	ImGui::Begin("About", &open_tabs[ABOUT]); 
	ImGui::PushFont(ui_fonts[REGULAR]);

	ImGui::Text("Kuroko Engine");
	ImGui::Separator();
	ImGui::Text("An engine to make videogames");
	ImGui::Text("By Rodrigo de Pedro Lombao and Lucas Garcia Mateu.");
	ImGui::Text("Kuroko Engine is licensed under the MIT License.\n");
	ImGui::Separator();

	ImGui::Text("Libraries used:");
	ImGui::Text("Assimp %i", ASSIMP_API::aiGetVersionMajor());
	ImGui::SameLine();
	if (ImGui::Button("Learn more##assimp"))
		App->requestBrowser("http://www.assimp.org/");
	ImGui::Text("Glew %s", glewGetString(GLEW_VERSION));
	ImGui::SameLine();
	if (ImGui::Button("Learn more##glew"))
		App->requestBrowser("http://glew.sourceforge.net/");
	ImGui::Text("DevIL 1.8.0");
	ImGui::SameLine();
	if (ImGui::Button("Learn more##devil"))
		App->requestBrowser("http://openil.sourceforge.net/");
	ImGui::Text("MathGeoLib ? version");
	ImGui::SameLine();
	if (ImGui::Button("Learn more##mathgeolib"))
		App->requestBrowser("http://clb.demon.fi/MathGeoLib/");
	SDL_version compiled; 
	SDL_GetVersion(&compiled);
	ImGui::Text("SDL %d.%d.%d", compiled.major, compiled.major, compiled.patch);
	ImGui::SameLine();
	if (ImGui::Button("Learn more##sdl"))
		App->requestBrowser("https://wiki.libsdl.org/FrontPage");
	ImGui::Text("OpenGL %s", glGetString(GL_VERSION));
	ImGui::Text("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::SameLine();
	if (ImGui::Button("Learn more##opngl"))
		App->requestBrowser("https://www.opengl.org/");
	ImGui::Text("Parson");
	ImGui::SameLine();
	if (ImGui::Button("Learn more##parson"))
		App->requestBrowser("https://github.com/kgabis/parson");
	ImGui::Text("PCG Random");
	ImGui::SameLine();
	if (ImGui::Button("Learn more##pcg_random"))
		App->requestBrowser("http://www.pcg-random.org");
	ImGui::Text("ImGuizmo");
	ImGui::SameLine();
	if(ImGui::Button("Learn more##imguizmo"))
		App->requestBrowser("https://github.com/CedricGuillemet/ImGuizmo");
	ImGui::PopFont();
	ImGui::End();
}

void ModuleUI::DrawGraphicsLeaf() const {
	//starting values
	ImGui::PushFont(ui_fonts[REGULAR]);

	static bool depth_test = glIsEnabled(GL_DEPTH_TEST);
	static bool face_culling = glIsEnabled(GL_CULL_FACE);
	static bool lighting = glIsEnabled(GL_LIGHTING);
	static bool material_color = glIsEnabled(GL_COLOR_MATERIAL);
	static bool textures = glIsEnabled(GL_TEXTURE_2D);
	static bool fog = glIsEnabled(GL_FOG);
	static bool antialias = glIsEnabled(GL_LINE_SMOOTH);
	ImGui::Text("Use this tab to enable/disable openGL characteristics");

	if (ImGui::TreeNode("Depth test")) {
		if (ImGui::Checkbox("Enabled##DT Enabled", &depth_test)) {
			if (depth_test)			glEnable(GL_DEPTH_TEST);
			else					glDisable(GL_DEPTH_TEST);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Face culling")) {
		if (ImGui::Checkbox("Enabled##FC Enabled", &face_culling)) {
			if (face_culling)		glEnable(GL_CULL_FACE);
			else					glDisable(GL_CULL_FACE);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Lighting")) {
		if (ImGui::Checkbox("Enabled##L Enabled", &lighting)) {
			if (lighting)			glEnable(GL_LIGHTING);
			else					glDisable(GL_LIGHTING);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Material color")) {
		if (ImGui::Checkbox("Enabled##M Enabled", &material_color)) {
			if (material_color)		glEnable(GL_COLOR_MATERIAL);
			else					glDisable(GL_COLOR_MATERIAL);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Textures")) {
		if (ImGui::Checkbox("Enabled##T Enabled", &textures)) {
			if (textures)			glEnable(GL_TEXTURE_2D);
			else					glDisable(GL_TEXTURE_2D);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Fog")) {
		static float fog_distance = 0.5f;
		if (ImGui::Checkbox("Enabled##F Enabled", &fog)) {
			if (fog)				glEnable(GL_FOG);
			else					glDisable(GL_FOG);

			if (fog) {
				GLfloat fog_color[4] = { 0.8f, 0.8f, 0.8f, 0.0f };
				glFogfv(GL_FOG_COLOR, fog_color);
				glFogf(GL_FOG_DENSITY, fog_distance);
			}
		}

		if (ImGui::SliderFloat("Fog density", &fog_distance, 0.0f, 1.0f))
			glFogf(GL_FOG_DENSITY, fog_distance);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Antialias")) {
		if (ImGui::Checkbox("Enabled##A Enabled", &antialias)) {
			if (antialias)			glEnable(GL_LINE_SMOOTH);
			else					glDisable(GL_LINE_SMOOTH);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Wireframe")) {
		ImGui::Checkbox("Enabled##WF Enabled", &App->scene->global_wireframe);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Normals")) {
		ImGui::Checkbox("Enabled##N Enabled", &App->scene->global_normals);
		ImGui::TreePop();
	}
	ImGui::PopFont();
}

void ModuleUI::DrawWindowConfigLeaf() const
{
	ImGui::PushFont(ui_fonts[REGULAR]);

	Window* window = App->window->main_window;
	if(ImGui::SliderFloat("Brightness", &window->brightness, 0, 1.0f))
		App->window->setBrightness(window->brightness);

	bool width_mod, height_mod = false;
	width_mod = ImGui::SliderInt("Width", &window->width, 640, 1920);
	height_mod = ImGui::SliderInt("Height", &window->height, 480, 1080);
	
	if(width_mod || height_mod)
		App->window->setSize(window->width, window->height);

	// Refresh rate
	ImGui::Text("Refresh Rate %i", (int)ImGui::GetIO().Framerate);
	//Bools
	if (ImGui::Checkbox("Fullscreen", &window->fullscreen))
		App->window->setFullscreen(window->fullscreen);
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &window->resizable))
		App->window->setResizable(window->resizable);
	if (ImGui::Checkbox("Borderless", &window->borderless))
		App->window->setBorderless(window->borderless);
	ImGui::SameLine();
	if (ImGui::Checkbox("FullDesktop", &window->fulldesk))
		App->window->setFullDesktop(window->fulldesk);

	ImGui::PopFont();
}

void ModuleUI::DrawHardwareLeaf() const 
{
	ImGui::PushFont(ui_fonts[REGULAR]);
	
	//CPUs
	ImGui::Text("CPUs");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255),"%i", SDL_GetCPUCount());

	// RAM
	ImGui::Text("System RAM");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%i Gb", SDL_GetSystemRAM());

	// Caps
	ImGui::Text("Caps:");
	ImGui::SameLine();
	if(SDL_HasRDTSC())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "RDTSC, ");
	ImGui::SameLine();
	if (SDL_HasMMX())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "MMX, ");
	ImGui::SameLine();
	if (SDL_HasSSE())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE, ");
	ImGui::SameLine();
	if (SDL_HasSSE2())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE2, ");
	if (SDL_HasSSE3())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE3, ");
	ImGui::SameLine();
	if (SDL_HasSSE41())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE41, ");
	ImGui::SameLine();
	if (SDL_HasSSE42())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "SSE42, ");
	ImGui::SameLine();
	if (SDL_HasAVX())
		ImGui::TextColored(ImVec4(0, 255, 0, 255), "AVX.");
	ImGui::SameLine();

	ImGui::Separator();
	//GPU
	ImGui::Text("Caps:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%s", glGetString(GL_VENDOR));
	ImGui::Text("Brand:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%s", glGetString(GL_RENDERER));
	ImGui::Text("VRAM Budget:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getTotalVRAMMb_NVIDIA());
	ImGui::Text("VRAM Usage:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getTotalVRAMMb_NVIDIA() - getAvaliableVRAMMb_NVIDIA());
	ImGui::Text("VRAM Avaliable:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", getAvaliableVRAMMb_NVIDIA());
	ImGui::Text("VRAM Reserved:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 255, 0, 255), "%f Mb", 0);
		
	ImGui::PopFont();
}

void ModuleUI::DrawApplicationLeaf() const 
{
	// HARDCODED (?)
	ImGui::PushFont(ui_fonts[REGULAR]);
	
	ImGui::Text("App name: Kuroko Engine");
	ImGui::Text("Organization: UPC CITM");
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	
	ImGui::PopFont();
}

void ModuleUI::DrawTimeControlWindow()
{
	ImGui::Begin("Time control", &open_tabs[TIME_CONTROL]);

	int w, h;
	ui_textures[PLAY]->getSize(w, h);
	if (ImGui::ImageButton((void*)ui_textures[PLAY]->getGLid(), ImVec2(w, h), nullptr, ImVec2(0,0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->time->getGameState() == PLAYING ? 1.0f : 0.0f)))
		App->time->Play();

	ImGui::SameLine();
	ui_textures[PAUSE]->getSize(w, h);
	if(ImGui::ImageButton((void*)ui_textures[PAUSE]->getGLid(), ImVec2(w, h), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->time->getGameState() == PAUSED ? 1.0f : 0.0f)))
		App->time->Pause();

	ImGui::SameLine();
	ui_textures[STOP]->getSize(w, h);
	if (ImGui::ImageButton((void*)ui_textures[STOP]->getGLid(), ImVec2(w, h), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->time->getGameState() == STOPPED ? 1.0f : 0.0f)))
		App->time->Stop();
	

	static int advance_frames = 1;
	static float time_scale = 1;

	ImGui::SameLine();
	ui_textures[ADVANCE]->getSize(w, h);
	if (ImGui::ImageButton((void*)ui_textures[ADVANCE]->getGLid(), ImVec2(w, h), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, 0.0f)))
		App->time->Advance(advance_frames);


	ImGui::DragInt("Advance frames", &advance_frames, 0.05, 1, 1000);


	if (ImGui::DragFloat("Time scale", &time_scale, 0.01, 0, 100))
		App->time->setTimeScale(time_scale);


	ImGui::Text("%f seconds in real time clock", App->time->getRealTime() / 1000);
	ImGui::Text("%f ms delta_time", App->time->getDeltaTime());
	ImGui::Text("%f seconds in game time clock", App->time->getGameTime() / 1000);
	ImGui::Text("%f ms game delta_time", App->time->getGameDeltaTime());
	ImGui::Text("%i frames", App->time->getFrameCount());




	ImGui::End();
}

void ModuleUI::DrawGizmoMenuTab() {

	ImGui::Begin("##Gizmo menu", nullptr);

	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_SELECT]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, !draw_guizmo ? 1.0f : 0.0f)))
		draw_guizmo = !draw_guizmo;
	
	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_TRANSLATE]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, gizmo_operation == ImGuizmo::OPERATION::TRANSLATE && draw_guizmo ? 1.0f : 0.0f)))
	{
		gizmo_operation = ImGuizmo::OPERATION::TRANSLATE;
		draw_guizmo = true;
	}

	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_ROTATE]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, gizmo_operation == ImGuizmo::OPERATION::ROTATE && draw_guizmo ? 1.0f : 0.0f)))
	{
		gizmo_operation = ImGuizmo::OPERATION::ROTATE;
		draw_guizmo = true;
	}

	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_SCALE]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, gizmo_operation == ImGuizmo::OPERATION::SCALE&& draw_guizmo ? 1.0f : 0.0f)))
	{
		gizmo_operation = ImGuizmo::OPERATION::SCALE;
		draw_guizmo = true;
	}

	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_LOCAL]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, gizmo_mode == ImGuizmo::MODE::LOCAL && draw_guizmo ? 1.0f : 0.0f)))
	{
		gizmo_mode = ImGuizmo::MODE::LOCAL;
		draw_guizmo = true;
	}

	if (ImGui::ImageButton((void*)ui_textures[GUIZMO_GLOBAL]->getGLid(), ImVec2(32, 32), nullptr, ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, gizmo_mode == ImGuizmo::MODE::WORLD && draw_guizmo ? 1.0f : 0.0f)))
	{
		gizmo_mode = ImGuizmo::MODE::WORLD;
		draw_guizmo = true;
	}

	ImGui::End();

}

void ModuleUI::DrawQuadtreeConfigWindow() {

	ImGui::Begin("Quadtree", &open_tabs[QUADTREE_CONFIG]);
	ImGui::Checkbox("Draw", &App->scene->draw_quadtree);
	ImGui::SameLine();
	if (ImGui::Button("Reload")) {
		App->scene->quadtree_reload = true;
	}
	static float3 size = float3(0,0,0);
	static float3 centre = float3(0, 0, 0);
	static int bucket_size = 1;
	static int max_depth = 8;
	static float size_arr[3] = { 0 };
	static float centre_arr[3] = { 0 };

	ImGui::InputFloat3("Centre", centre_arr);
	centre.x = centre_arr[0];
	centre.y = centre_arr[1];
	centre.z = centre_arr[2];

	ImGui::InputFloat3("Size", size_arr);
	size.x = size_arr[0];
	size.y = size_arr[1];
	size.z = size_arr[2];

	ImGui::InputInt("Bucket size", &bucket_size);
	ImGui::InputInt("Max depth", &max_depth);

	if (ImGui::Button("Create")) {
		AABB aabb;
		aabb.SetFromCenterAndSize(centre, size);
		App->scene->getQuadtree()->Create(aabb, bucket_size, max_depth);
		App->scene->quadtree_reload = true;
	}
	ImGui::Text("Quadtree ignored objects: %i",App->scene->quadtree_ignored_obj);
	// TODO: Be able to change bucket size, max depth and size.
	ImGui::End();
}


void ModuleUI::DrawGuizmo()
{
	App->gui->DrawGizmoMenuTab();

	if (draw_guizmo)
	{
		ImGuizmo::BeginFrame();
		float4x4 projection4x4;
		float4x4 view4x4;

		glGetFloatv(GL_MODELVIEW_MATRIX, (float*)view4x4.v);
		glGetFloatv(GL_PROJECTION_MATRIX, (float*)projection4x4.v);

		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ComponentTransform* transform = (ComponentTransform*)App->scene->selected_obj->getComponent(TRANSFORM);
		if (transform->getMode() == LOCAL)
			transform->LocalToGlobal();
		else
			transform->global->CalculateMatrix();

		Transform aux_transform;
		switch (gizmo_operation)
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			aux_transform.setRotation(transform->global->getRotation());
			aux_transform.setPosition(transform->global->getPosition()); break;
		case ImGuizmo::OPERATION::ROTATE:
			aux_transform.setPosition(transform->global->getPosition());
			aux_transform.setRotation(transform->global->getRotation()); break;
		case ImGuizmo::OPERATION::SCALE:
			aux_transform.setPosition(transform->global->getPosition());
			aux_transform.setRotation(transform->global->getRotation());
			aux_transform.setScale(transform->global->getScale()); break;
		default:
			break;
		}
		
		aux_transform.CalculateMatrix();
		float4x4 mat = float4x4(aux_transform.getMatrix());
		mat.Transpose();
		ImGuizmo::Manipulate((float*)view4x4.v, (float*)projection4x4.v, gizmo_operation, gizmo_mode, (float*)mat.v);
		if (ImGuizmo::IsUsing())
		{
			float3 new_pos = float3::zero;
			float3 new_rot = float3::zero;
			float3 new_scale = float3::zero;
			mat.Transpose();
			switch (gizmo_operation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				new_pos.x = transform->constraints[0][0] ? transform->global->getPosition().x : mat.TranslatePart().x;
				new_pos.y = transform->constraints[0][1] ? transform->global->getPosition().y : mat.TranslatePart().y;
				new_pos.z = transform->constraints[0][2] ? transform->global->getPosition().z : mat.TranslatePart().z;
				transform->global->setPosition(new_pos);
				break;
			case ImGuizmo::OPERATION::ROTATE:
				new_rot.x = transform->constraints[1][0] ? transform->global->getRotationEuler().x : mat.RotatePart().ToEulerXYZ().x;
				new_rot.y = transform->constraints[1][1] ? transform->global->getRotationEuler().y : mat.RotatePart().ToEulerXYZ().y;
				new_rot.z = transform->constraints[1][2] ? transform->global->getRotationEuler().z : mat.RotatePart().ToEulerXYZ().z;
				transform->global->setRotation(Quat::FromEulerXYZ(new_rot.x, new_rot.y, new_rot.z)); 
				break;
			case ImGuizmo::OPERATION::SCALE:
				new_scale.x = transform->constraints[2][0] ? transform->global->getScale().x : mat.GetScale().x;
				new_scale.y = transform->constraints[2][1] ? transform->global->getScale().y : mat.GetScale().y;
				new_scale.z = transform->constraints[2][2] ? transform->global->getScale().z : mat.GetScale().z;
				transform->global->setScale(new_scale);
				break;
			default:
				break;
			}
			transform->global->CalculateMatrix();
			transform->GlobalToLocal();
		}
	}
}

void ModuleUI::SaveConfig(JSON_Object* config) const
{
	json_object_set_boolean(config, "hierarchy", open_tabs[HIERARCHY]);
	json_object_set_boolean(config, "obj_inspector", open_tabs[OBJ_INSPECTOR]);
	json_object_set_boolean(config, "primitive", open_tabs[PRIMITIVE]);
	json_object_set_boolean(config, "about", open_tabs[ABOUT]);
	json_object_set_boolean(config, "configuration", open_tabs[CONFIGURATION]);
	json_object_set_boolean(config, "log", open_tabs[LOG]);
	json_object_set_boolean(config, "time_control", open_tabs[TIME_CONTROL]);
	json_object_set_boolean(config, "quadtree_config", open_tabs[QUADTREE_CONFIG]);
	json_object_set_boolean(config, "camera_menu", open_tabs[CAMERA_MENU]);
	json_object_set_boolean(config, "asset_window", open_tabs[ASSET_WINDOW]);
	json_object_set_boolean(config, "resources_window", open_tabs[RESOURCES_TAB]);
	//json_object_set_boolean(config, "audio", open_tabs[AUDIO]);
}

void ModuleUI::LoadConfig(const JSON_Object* config) 
{
	open_tabs[CONFIGURATION]	= json_object_get_boolean(config, "configuration");
	open_tabs[HIERARCHY]		= json_object_get_boolean(config, "hierarchy");
	open_tabs[OBJ_INSPECTOR]	= json_object_get_boolean(config, "obj_inspector");
	open_tabs[PRIMITIVE]		= json_object_get_boolean(config, "primitive");
	open_tabs[ABOUT]			= json_object_get_boolean(config, "about");
	open_tabs[LOG]				= json_object_get_boolean(config, "log");
	open_tabs[TIME_CONTROL]		= json_object_get_boolean(config, "time_control");
	open_tabs[QUADTREE_CONFIG]	= json_object_get_boolean(config, "quadtree_config");
	open_tabs[CAMERA_MENU]		= json_object_get_boolean(config, "camera_menu");
	open_tabs[ASSET_WINDOW]		= json_object_get_boolean(config, "asset_window");
	open_tabs[RESOURCES_TAB]	= json_object_get_boolean(config, "resources_window");

	open_tabs[VIEWPORT_MENU]	= false;	// must always start closed
	//open_tabs[AUDIO]			= json_object_get_boolean(config, "audio");
}

void ModuleUI::InvisibleDockingBegin() {
	ImGuiWindowFlags window = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	static ImGuiDockNodeFlags optional = ImGuiDockNodeFlags_PassthruDockspace;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::SetNextWindowBgAlpha(0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Kuroko Engine", &docking_background, window);
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGuiIO& io = ImGui::GetIO();

	ImGuiID dockspace_id = ImGui::GetID("The dockspace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), optional);
}

void ModuleUI::InvisibleDockingEnd() {
	ImGui::End();
}