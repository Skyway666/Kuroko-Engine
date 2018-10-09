 #include "ModuleImGUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"
#include "ModuleAudio.h"
#include "Applog.h"

#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentAABB.h"

#include "RNG.h"
#include "VRAM.h"
#include "WinItemDialog.h" 

#include "Assimp/include/version.h"

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment( lib, "glew-2.1.0/lib/glew32.lib")
#pragma comment( lib, "glew-2.1.0/lib/glew32s.lib")


ModuleImGUI::ModuleImGUI(Application* app, bool start_enabled) : Module(app, start_enabled) {
	name = "gui";
}


ModuleImGUI::~ModuleImGUI() {
}

bool ModuleImGUI::Init(JSON_Object* config) {
	
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

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
	
	LoadConfig(config);


	return true;
}

bool ModuleImGUI::Start()
{
	if (App->importer->Import("Play.png"), I_TEXTURE)			ui_textures[PLAY] = App->importer->getLastTex();
	if (App->importer->Import("Pause.png"), I_TEXTURE)			ui_textures[PAUSE] = App->importer->getLastTex();
	if (App->importer->Import("Stop.png"), I_TEXTURE)			ui_textures[STOP] = App->importer->getLastTex();
	if (App->importer->Import("no_texture.png"), I_TEXTURE)		ui_textures[NO_TEXTURE] = App->importer->getLastTex();

	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	docking_background = true;

	return true;
}

update_status ModuleImGUI::PreUpdate(float dt) {

	// Start the ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->main_window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleImGUI::Update(float dt) {


	InvisibleDockingBegin();
	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	if(open_tabs[TEST])
	{

	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.

	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN) 
		open_tabs[CONFIGURATION] = !open_tabs[CONFIGURATION];


	if (open_tabs[CONFIGURATION]) {
		ImGui::Begin("Configuration", &open_tabs[CONFIGURATION]);
		if (ImGui::CollapsingHeader("Graphics")) 
			DrawGraphicsTab();
		if (ImGui::CollapsingHeader("Window"))
			DrawWindowConfig();
		if (ImGui::CollapsingHeader("Hardware"))
			DrawHardware();
		if (ImGui::CollapsingHeader("Application"))
			DrawApplication();;
		ImGui::End();
	}

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (open_tabs[DEMO]) {
		ImGui::SetNextWindowPos(ImVec2(650, 320), ImGuiCond_FirstUseEver); 
		ImGui::ShowDemoWindow(&open_tabs[DEMO]);
	}

	if (open_tabs[HIERARCHY])
	{
		ImGui::SetNextWindowPos(ImVec2(0, 220), ImGuiCond_FirstUseEver); 
		DrawHierarchyTab();
	}

	if (open_tabs[OBJ_INSPECTOR])
	{
		ImGui::SetNextWindowPos(ImVec2(700, 320), ImGuiCond_FirstUseEver); 
		DrawObjectInspectorTab();
	}

	if (open_tabs[PRIMITIVE])
	{
		ImGui::SetNextWindowPos(ImVec2(400, 320), ImGuiCond_FirstUseEver);
		DrawPrimitivesTab();
	}
	if (open_tabs[ABOUT])
	{
		//ImGui::SetNextWindowPos(ImVec2(?, ?), ImGuiCond_FirstUseEver);
		DrawAboutWindow();
	}
	
	if (open_tabs[LOG])
		app_log->Draw("App log",&open_tabs[LOG]);

	if (open_tabs[TIME_CONTROL])
		DrawTimeControl();

	if (open_tabs[AUDIO])
		DrawAudioTab();


	bool close_app = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit"))
				close_app = true;
			if (ImGui::MenuItem("Import file"))
			{
				std::string file_path = openFileWID();
				App->importer->Import(file_path.c_str());
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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			ImGui::MenuItem("Demo", NULL, &open_tabs[DEMO]);
			ImGui::MenuItem("Test", NULL, &open_tabs[TEST]);
			ImGui::MenuItem("Hierarchy", NULL, &open_tabs[HIERARCHY]);
			ImGui::MenuItem("Object Inspector", NULL, &open_tabs[OBJ_INSPECTOR]);
			ImGui::MenuItem("Primitive", NULL, &open_tabs[PRIMITIVE]);
			ImGui::MenuItem("Configuration", NULL, &open_tabs[CONFIGURATION]);
			ImGui::MenuItem("Log", NULL, &open_tabs[LOG]);
			ImGui::MenuItem("Time control", NULL, &open_tabs[TIME_CONTROL]);
			ImGui::MenuItem("Audio", NULL, &open_tabs[AUDIO]);
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
	
	}
	ImGui::EndMainMenuBar();
	ImGui::End();
	if (!close_app)
		return UPDATE_CONTINUE;
	else
		return UPDATE_STOP;

}

update_status ModuleImGUI::PostUpdate(float dt) {
	// Rendering
	ImGui::Render();
	
	glViewport(0, 0, (int)io->DisplaySize.x, (int)io->DisplaySize.y);

	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());


	return UPDATE_CONTINUE;
}

bool ModuleImGUI::CleanUp() {
	// Cleanup
	for (int i = 0; i < LAST_UI_TEX; i++)
	{
		delete ui_textures[i];
		ui_textures[i] = nullptr;
	}
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}




void ModuleImGUI::DrawHierarchyTab()
{
	ImGui::Begin("Hierarchy Tab", &open_tabs[HIERARCHY]);
	ImGui::Text("Use this tab to set the hierarchy of the scene objects");
	int id = 0;

	//for (std::list<GameObject*>::iterator it = App->scene_intro->game_objects.begin(); it != App->scene_intro->game_objects.end(); it++)
	//	DrawHierarchyNode(*it, id);

	//Just for assignment 1
	if (!App->scene_intro->game_objects.empty()) {
		std::list<GameObject*>::iterator it = App->scene_intro->game_objects.end();
		it--;
		DrawHierarchyNode(*it, id);
	}

	ImGui::End();
}

void ModuleImGUI::DrawHierarchyNode(GameObject* game_object, int& id)
{
	id++;
	static int selection_mask = (1 << 2);
	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << id)) ? ImGuiTreeNodeFlags_Selected : 0);

	std::list<GameObject*> children;
	game_object->getChildren(children);

	if(children.empty())
		node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; 

	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)id, node_flags, game_object->getName().c_str(), id) && !children.empty();

	if (ImGui::IsItemClicked())
	{
		selection_mask = (1 << id);
		App->scene_intro->selected_obj = game_object;
	}

	if (node_open)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3);

		for (std::list<GameObject*>::iterator it = children.begin(); it != children.end(); it++)
				DrawHierarchyNode(*it, id);

		ImGui::PopStyleVar();
		ImGui::TreePop();
	}
}

void ModuleImGUI::DrawObjectInspectorTab()
{
	ImGui::Begin("Object inspector", &open_tabs[OBJ_INSPECTOR]);
	ImGui::Text("Use this tab to add, edit and remove components of gameobjects");

	static bool show_rename = false;
	GameObject* selected_obj = App->scene_intro->selected_obj;

	if (selected_obj)
	{
		ImGui::Text("Name: %s", selected_obj->getName().c_str());

		ImGui::SameLine();
		if (ImGui::Button("Rename"))
			show_rename = true;

		ImGui::Checkbox("Active", &selected_obj->is_active);
		ImGui::SameLine();
		ImGui::Checkbox("Static", &selected_obj->is_static);

		if (ImGui::CollapsingHeader("Add component"))
		{
			if (ImGui::Button("Add Mesh"))	selected_obj->addComponent(MESH);
			if (ImGui::Button("Add AABB"))  selected_obj->addComponent(C_AABB);
		}

		std::list<Component*> components;
		selected_obj->getComponents(components);

		std::list<Component*> components_to_erase;
		for (std::list<Component*>::iterator it = components.begin(); it != components.end(); it++)
			if (!DrawComponent(*it))
				components_to_erase.push_back(*it);

		for (std::list<Component*>::iterator it = components_to_erase.begin(); it != components_to_erase.end(); it++)
			selected_obj->removeComponent(*it);
	}
	else if (show_rename)
		show_rename = false;

	ImGui::End();
	
	if (show_rename)
	{
		ImGui::SetNextWindowPos(ImVec2(700, 320), ImGuiCond_FirstUseEver); 
		ImGui::Begin("Rename object");
		static char rename_buffer[64];
		ImGui::InputText("Rename to", rename_buffer, 64);

		ImGui::SameLine();
		if (ImGui::Button("Change"))
		{
			selected_obj->Rename(rename_buffer);
			show_rename = false;
		}

		ImGui::End();
	}
}

bool ModuleImGUI::DrawComponent(Component* component)
{
	switch (component->getType())
	{
	case MESH:
		if (ImGui::CollapsingHeader("Mesh"))
		{
			ComponentMesh* c_mesh = (ComponentMesh*)component;
			static bool wireframe_enabled;
			static bool mesh_active;
			static bool draw_normals;

			wireframe_enabled = c_mesh->getWireframe();
			draw_normals = c_mesh->getDrawNormals();
			mesh_active = c_mesh->isActive();

			if (ImGui::Checkbox("Is active", &mesh_active))
				c_mesh->setActive(mesh_active);

			if (mesh_active)
			{

				if (ImGui::Checkbox("Wireframe", &wireframe_enabled))
					c_mesh->setWireframe(wireframe_enabled);

				if (ImGui::Checkbox("Draw normals", &draw_normals))
					c_mesh->setDrawNormals(draw_normals);
				
				
				if (Material* material = c_mesh->getMaterial())
				{
					if (ImGui::TreeNode("Material"))
					{
						static int preview_size = 64;
						ImGui::Text("Id: %d", material->getId());
						ImGui::Text("preview size");
						ImGui::SameLine();
						if (ImGui::Button("64")) preview_size = 64;
						ImGui::SameLine();
						if (ImGui::Button("128")) preview_size = 128;
						ImGui::SameLine();
						if (ImGui::Button("256")) preview_size = 256;

						if (ImGui::Button("remove material"))
						{
							c_mesh->setMaterial(nullptr);
							ImGui::TreePop();
							return true;
						}

						ImGui::Text("diffuse texture:  ");
						ImGui::SameLine();

						ImGui::Image(material->getTexture(DIFFUSE) ? (void*)material->getTexture(DIFFUSE)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

						if (ImGui::Button("Dif: Load checkered texture"))
							material->setCheckeredTexture(DIFFUSE);
						ImGui::SameLine();
						if (ImGui::Button("Dif: Load texture"))
						{
							std::string texture_path = openFileWID();
							if (App->importer->Import(texture_path.c_str(), I_TEXTURE))
								c_mesh->getMaterial()->setTexture(DIFFUSE, App->importer->getLastTex());
						}

						ImGui::Text("ambient texture:  ");

						ImGui::SameLine();
						ImGui::Image(material->getTexture(AMBIENT) ? (void*)material->getTexture(AMBIENT)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

						if (ImGui::Button("Amb: Load checkered texture"))
							material->setCheckeredTexture(AMBIENT);
						ImGui::SameLine();
						if (ImGui::Button("Amb: Load texture"))
						{
							std::string texture_path = openFileWID();
							if (App->importer->Import(texture_path.c_str(), I_TEXTURE))
								c_mesh->getMaterial()->setTexture(AMBIENT, App->importer->getLastTex());
						}

						ImGui::Text("normals texture:  ");

						ImGui::SameLine();
						ImGui::Image(material->getTexture(NORMALS) ? (void*)material->getTexture(NORMALS)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

						if (ImGui::Button("Nor: Load checkered texture"))
							material->setCheckeredTexture(NORMALS);
						ImGui::SameLine();
						if (ImGui::Button("Nor: Load texture"))
						{
							std::string texture_path = openFileWID();
							if (App->importer->Import(texture_path.c_str(), I_TEXTURE))
								c_mesh->getMaterial()->setTexture(NORMALS, App->importer->getLastTex());
						}

						ImGui::Text("lightmap texture:  ");

						ImGui::SameLine();
						ImGui::Image(material->getTexture(LIGHTMAP) ? (void*)material->getTexture(LIGHTMAP)->getGLid() : (void*)ui_textures[NO_TEXTURE]->getGLid(), ImVec2(preview_size, preview_size));

						if (ImGui::Button("Lgm: Load checkered texture"))
							material->setCheckeredTexture(LIGHTMAP);
						ImGui::SameLine();
						if (ImGui::Button("Lgm: Load texture"))
						{
							std::string texture_path = openFileWID();
							if(App->importer->Import(texture_path.c_str(), I_TEXTURE))
								c_mesh->getMaterial()->setTexture(LIGHTMAP, App->importer->getLastTex());
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
						App->scene_intro->materials.push_back(mat);
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
						ImGui::SameLine();
						ImGui::Text(has_normals ? "normals: Yes," : "normals: No,");
						ImGui::SameLine();
						ImGui::Text(has_colors ? "colors: Yes," : "colors: No,");
						ImGui::SameLine();
						ImGui::Text(has_texcoords ? "tex coords: Yes" : "tex coords: No");

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Remove mesh"))
				return false;
		}
		break;
	case TRANSFORM:
		if (ImGui::CollapsingHeader("Transform"))
		{
			ImGui::Text("Drag the parameters to change them, or ctrl+click on one of them to set it's value");
			ComponentTransform* transform = (ComponentTransform*)component;

			static Vector3f position;
			static Vector3f rotation;
			static Vector3f scale;

			position = transform->position;
			rotation = transform->getRotationEuler();
			scale = transform->scale;


			//position
			ImGui::Text("Position:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("p x", &position.x, 0.01f, 0.0f, 0.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("p y", &position.y, 0.01f, 0.0f, 0.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("p z", &position.z, 0.01f, 0.0f, 0.0f, "%.02f");

			//rotation
			ImGui::Text("Rotation:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("r x", &rotation.x, 0.2f, -180.0f, 180.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("r y", &rotation.y, 0.2f, -180.0f, 180.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("r z", &rotation.z, 0.2f, -180.0f, 180.0f, "%.02f");

			//scale
			ImGui::Text("   Scale:");
			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("s x", &scale.x, 0.01f, -1000.0f, 1000.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("s y", &scale.y, 0.01f, -1000.0f, 1000.0f, "%.02f");

			ImGui::SameLine();
			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
			ImGui::DragFloat("s z", &scale.z, 0.01f, -1000.0f, 1000.0f, "%.02f");

			transform->setPosition(position);
			transform->setRotationEuler(rotation);
			transform->setScale(scale);
		}
		break;

	case C_AABB:
		if (ImGui::CollapsingHeader("AABB"))
		{
			ComponentAABB* aabb = (ComponentAABB*)component;

			static bool aabb_active;
			aabb_active = aabb->isActive();


			if (ImGui::Checkbox("active AABB", &aabb_active))
				aabb->setActive(aabb_active);

			if (aabb_active)
			{
				static bool aabb_drawn;
				aabb_drawn = aabb->draw_aabb;

				if (ImGui::Checkbox("AABB drawn", &aabb_drawn))
					aabb->draw_aabb = aabb_drawn;

				static bool obb_drawn;
				obb_drawn = aabb->draw_obb;

				if (ImGui::Checkbox("OBB drawn", &obb_drawn))
					aabb->draw_obb = obb_drawn;

				if (ImGui::Button("Reload AABB"))
					aabb->Reload();
			}

			if (ImGui::Button("Remove AABB"))
				return false;
		}
		break;
	default:
		break;
	}

	return true;
}

void ModuleImGUI::DrawAudioTab()
{
	ImGui::Begin("Audio", &open_tabs[AUDIO]);
	ImGui::Text("Use this tab to check and play loaded audio files");
	
	for (auto it = App->audio->audio_files.begin(); it != App->audio->audio_files.end(); it++)
	{
		if (ImGui::TreeNode((*it)->name.c_str()))
		{
			ImGui::Text("type: %s", (*it)->type == FX ? "FX" : "Music");
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)ui_textures[PLAY]->getGLid(), ImVec2(16, 16)))
				(*it)->Play();
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)ui_textures[STOP]->getGLid(), ImVec2(16, 16)))
				(*it)->Stop();
			ImGui::TreePop();
		}
	}

	ImGui::End();
}

void ModuleImGUI::DrawPrimitivesTab()
{
	ImGui::Begin("Primitives", &open_tabs[PRIMITIVE]);
	ImGui::Text("Use this tab to add primitives to the scene");

	if (ImGui::Button("Add cube"))
	{
		GameObject* cube = new GameObject("Cube");
		Mesh* mesh = new Mesh(Primitive_Cube);
		cube->addComponent(new ComponentMesh(cube, mesh));
		App->scene_intro->game_objects.push_back(cube);
	}
	if (ImGui::Button("Add plane"))
	{
		GameObject* plane = new GameObject("Plane");
		Mesh* mesh = new Mesh(Primitive_Plane);
		plane->addComponent(new ComponentMesh(plane, mesh));
		App->scene_intro->game_objects.push_back(plane);
	}
	if (ImGui::Button("Add sphere")) {
		GameObject* sphere = new GameObject("Sphere");
		Mesh* mesh = new Mesh(Primitive_Sphere);
		sphere->addComponent(new ComponentMesh(sphere, mesh));
		App->scene_intro->game_objects.push_back(sphere);
	}
	if (ImGui::Button("Add cylinder")) {
		GameObject* cylinder = new GameObject("Cylinder");
		Mesh* mesh = new Mesh(Primitive_Cylinder);
		cylinder->addComponent(new ComponentMesh(cylinder, mesh));
		App->scene_intro->game_objects.push_back(cylinder);
	}

	ImGui::End();
}


void ModuleImGUI::DrawAboutWindow() {
	ImGui::Begin("About", &open_tabs[ABOUT]);
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
	
	ImGui::End();
}

void ModuleImGUI::DrawGraphicsTab() {
	//starting values

	static bool depth_test = glIsEnabled(GL_DEPTH_TEST);
	static bool face_culling = glIsEnabled(GL_CULL_FACE);
	static bool lighting = glIsEnabled(GL_LIGHTING);
	static bool material_color = glIsEnabled(GL_COLOR_MATERIAL);
	static bool textures = glIsEnabled(GL_TEXTURE_2D);
	static bool fog = glIsEnabled(GL_FOG);
	static bool antialias = glIsEnabled(GL_LINE_SMOOTH);
	ImGui::Text("Use this tab to enable/disable openGL characteristics");

	if (ImGui::TreeNode("Depth test")) {
		if (ImGui::Checkbox("DT Enabled", &depth_test)) {
			if (depth_test)			glEnable(GL_DEPTH_TEST);
			else					glDisable(GL_DEPTH_TEST);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Face culling")) {
		if (ImGui::Checkbox("FC Enabled", &face_culling)) {
			if (face_culling)		glEnable(GL_CULL_FACE);
			else					glDisable(GL_CULL_FACE);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Lighting")) {
		if (ImGui::Checkbox("L Enabled", &lighting)) {
			if (lighting)			glEnable(GL_LIGHTING);
			else					glDisable(GL_LIGHTING);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Material color")) {
		if (ImGui::Checkbox("M Enabled", &material_color)) {
			if (material_color)		glEnable(GL_COLOR_MATERIAL);
			else					glDisable(GL_COLOR_MATERIAL);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Textures")) {
		if (ImGui::Checkbox("T Enabled", &textures)) {
			if (textures)			glEnable(GL_TEXTURE_2D);
			else					glDisable(GL_TEXTURE_2D);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Fog")) {
		static float fog_distance = 0.5f;
		if (ImGui::Checkbox("F Enabled", &fog)) {
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
		if (ImGui::Checkbox("A Enabled", &antialias)) {
			if (antialias)			glEnable(GL_LINE_SMOOTH);
			else					glDisable(GL_LINE_SMOOTH);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Wireframe")) {
		ImGui::Checkbox("WF Enabled", &App->scene_intro->global_wireframe);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Normals")) {
		ImGui::Checkbox("N Enabled", &App->scene_intro->global_normals);
		ImGui::TreePop();
	}
}

void ModuleImGUI::DrawWindowConfig() {
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

}

void ModuleImGUI::DrawHardware() {
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
}

void ModuleImGUI::DrawApplication(){
	// HARDCODED (?)
	ImGui::Text("App name: Kuroko Engine");
	ImGui::Text("Organization: UPC CITM");
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
}

void ModuleImGUI::DrawTimeControl()
{
	ImGui::Begin("Time control", &open_tabs[TIME_CONTROL]);

	int w, h;
	ui_textures[PLAY]->getSize(w, h);
	if (ImGui::ImageButton((void*)ui_textures[PLAY]->getGLid(), ImVec2(w, h), ImVec2(0,0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->scene_intro->game_state == PLAYING ? 1.0f : 0.0f)))
		App->scene_intro->Play();

	ImGui::SameLine();
	ui_textures[PAUSE]->getSize(w, h);
	if(ImGui::ImageButton((void*)ui_textures[PAUSE]->getGLid(), ImVec2(w, h), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->scene_intro->game_state == PAUSED ? 1.0f : 0.0f)))
		App->scene_intro->Pause();

	ImGui::SameLine();
	ui_textures[STOP]->getSize(w, h);
	if (ImGui::ImageButton((void*)ui_textures[STOP]->getGLid(), ImVec2(w, h), ImVec2(0, 0), ImVec2(1, 1), 0, ImVec4(0.0f, 0.7f, 0.7f, App->scene_intro->game_state == STOPPED ? 1.0f : 0.0f)))
		App->scene_intro->Stop();

	ImGui::End();
}

void ModuleImGUI::SaveConfig(JSON_Object* config) 
{
	json_object_set_boolean(config, "demo", open_tabs[DEMO]);
	json_object_set_boolean(config, "test", open_tabs[TEST]);
	json_object_set_boolean(config, "hierarchy", open_tabs[HIERARCHY]);
	json_object_set_boolean(config, "obj_inspector", open_tabs[OBJ_INSPECTOR]);
	json_object_set_boolean(config, "primitive", open_tabs[PRIMITIVE]);
	json_object_set_boolean(config, "about", open_tabs[ABOUT]);
	json_object_set_boolean(config, "configuration", open_tabs[CONFIGURATION]);
	json_object_set_boolean(config, "log", open_tabs[LOG]);
	json_object_set_boolean(config, "time_control", open_tabs[TIME_CONTROL]);
	json_object_set_boolean(config, "audio", open_tabs[AUDIO]);
}

void ModuleImGUI::LoadConfig(JSON_Object* config) 
{
	open_tabs[DEMO]				= json_object_get_boolean(config, "demo");
	open_tabs[CONFIGURATION]	= json_object_get_boolean(config, "configuration");
	open_tabs[TEST]				= json_object_get_boolean(config, "test");
	open_tabs[HIERARCHY]		= json_object_get_boolean(config, "hierarchy");
	open_tabs[OBJ_INSPECTOR]	= json_object_get_boolean(config, "obj_inspector");
	open_tabs[PRIMITIVE]		= json_object_get_boolean(config, "primitive");
	open_tabs[ABOUT]			= json_object_get_boolean(config, "about");
	open_tabs[LOG]				= json_object_get_boolean(config, "log");
	open_tabs[TIME_CONTROL]		= json_object_get_boolean(config, "time_control");
	open_tabs[AUDIO]			= json_object_get_boolean(config, "audio");
}

void ModuleImGUI::InvisibleDockingBegin() {
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

void ModuleImGUI::InvisibleDockingEnd() {
	ImGui::End();
}