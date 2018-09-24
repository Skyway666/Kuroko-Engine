 #include "ModuleImGUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleImporter.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"

#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ComponentAABB.h"

#include "RNG.h"
#include "VRAM.h"

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
	io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->getContext());
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

update_status ModuleImGUI::PreUpdate(float dt) {

	// Start the ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleImGUI::Update(float dt) {


	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	if(open_tabs[TEST])
	{

		// test functionality
		static float f = 0.0f;
		static int random = 0;
		static float randomFloa = 0;
		static int bound1 = 0;
		static int bound2 = 100;
		ImGui::Text("RANDOM NUMBER GENERATOR");					 								    // Edit 1 float using a slider from 0.0f to 1.0f   
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		ImGui::DragInt("Start", &bound1);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
		ImGui::DragInt("End", &bound2);
		if (ImGui::Button("Generate random integer"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
			random = randomFromTo(bound1, bound2);

		ImGui::SameLine();
		ImGui::Text("Random integer = %d", random);

		if (ImGui::Button("Generate random float"))
			randomFloa = randomFloat();

		ImGui::SameLine();
		ImGui::Text("Random float = %f", randomFloa);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) 
		open_tabs[GRAPHIC] = !open_tabs[GRAPHIC];

	if (open_tabs[GRAPHIC]) DrawGraphicsTab();

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
	if (open_tabs[IMPORTER])
	{
		ImGui::SetNextWindowPos(ImVec2(600, 520), ImGuiCond_FirstUseEver);
		DrawImporterTab();
	}
	if (open_tabs[ABOUT])
	{
		//ImGui::SetNextWindowPos(ImVec2(?, ?), ImGuiCond_FirstUseEver);
		DrawAboutWindow();
	}
	if (open_tabs[WINDOW_CONFIG]) {
		//ImGui::SetNextWindowPos(ImVec2(?, ?), ImGuiCond_FirstUseEver);
		DrawWindowConfig();
	}
	if (open_tabs[HARDWARE]) {
		DrawHardware();
	}
	if (open_tabs[APPLICATION]) {
		DrawApplication();
	}


	bool close_app = false;

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit"))
				close_app = true;
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
			ImGui::MenuItem("Graphic", NULL, &open_tabs[GRAPHIC]);
			ImGui::MenuItem("Test", NULL, &open_tabs[TEST]);
			ImGui::MenuItem("Hierarchy", NULL, &open_tabs[HIERARCHY]);
			ImGui::MenuItem("Object Inspector", NULL, &open_tabs[OBJ_INSPECTOR]);
			ImGui::MenuItem("Primitive", NULL, &open_tabs[PRIMITIVE]);
			ImGui::MenuItem("Importer", NULL, &open_tabs[IMPORTER]);
			ImGui::MenuItem("Window", NULL, &open_tabs[WINDOW_CONFIG]);
			ImGui::MenuItem("Hardware", NULL, &open_tabs[HARDWARE]);
			ImGui::MenuItem("Application", NULL, &open_tabs[APPLICATION]);
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

	if (!close_app)
		return UPDATE_CONTINUE;
	else
		return UPDATE_STOP;

}

update_status ModuleImGUI::PostUpdate(float dt) {
	// Rendering
	ImGui::Render();
	
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
/*	glUseProgram(0);*/ // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

bool ModuleImGUI::CleanUp() {
	// Cleanup
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	return true;
}

void ModuleImGUI::DrawGraphicsTab()
{
	//starting values

	static bool depth_test			= glIsEnabled(GL_DEPTH_TEST);
	static bool face_culling		= glIsEnabled(GL_CULL_FACE);
	static bool lighting			= glIsEnabled(GL_LIGHTING);
	static bool material_color		= glIsEnabled(GL_COLOR_MATERIAL);
	static bool textures			= glIsEnabled(GL_TEXTURE_2D);
	static bool fog					= glIsEnabled(GL_FOG);
	static bool antialias			= glIsEnabled(GL_LINE_SMOOTH);


	ImGui::Begin("Graphics Tab", &open_tabs[GRAPHIC]);
	ImGui::Text("Use this tab to enable/disable openGL characteristics");

	if (ImGui::CollapsingHeader("Depth test"))
	{
		if (ImGui::Checkbox("DT Enabled", &depth_test))
		{
			if (depth_test)			glEnable(GL_DEPTH_TEST);
			else					glDisable(GL_DEPTH_TEST);
		}
	}
	if (ImGui::CollapsingHeader("Face culling"))
	{
		if (ImGui::Checkbox("FC Enabled", &face_culling))
		{
			if (face_culling)		glEnable(GL_CULL_FACE);
			else					glDisable(GL_CULL_FACE);
		}
	}
	if (ImGui::CollapsingHeader("Lighting"))
	{
		if (ImGui::Checkbox("L Enabled", &lighting))
		{
			if (lighting)			glEnable(GL_LIGHTING);
			else					glDisable(GL_LIGHTING);
		}
	}
	if (ImGui::CollapsingHeader("Material color"))
	{
		if (ImGui::Checkbox("M Enabled", &material_color))
		{
			if (material_color)		glEnable(GL_COLOR_MATERIAL);
			else					glDisable(GL_COLOR_MATERIAL);
		}
	}
	if (ImGui::CollapsingHeader("Textures"))
	{
		if (ImGui::Checkbox("T Enabled", &textures))
		{
			if (textures)			glEnable(GL_TEXTURE_2D);
			else					glDisable(GL_TEXTURE_2D);
		}
	}
	if (ImGui::CollapsingHeader("Fog"))
	{
		static float fog_distance = 0.5f;
		if (ImGui::Checkbox("F Enabled", &fog))
		{
			if (fog)				glEnable(GL_FOG);
			else					glDisable(GL_FOG);

			if (fog)
			{
				GLfloat fog_color[4] = { 0.8f, 0.8f, 0.8f, 0.0f };
				glFogfv(GL_FOG_COLOR, fog_color);
				glFogf(GL_FOG_DENSITY, fog_distance);
			}
		}

		if (ImGui::SliderFloat("Fog density", &fog_distance, 0.0f, 1.0f))
			glFogf(GL_FOG_DENSITY, fog_distance);
	}
	if (ImGui::CollapsingHeader("Antialias"))
	{
		if (ImGui::Checkbox("A Enabled", &antialias))
		{
			if (antialias)			glEnable(GL_LINE_SMOOTH);
			else					glDisable(GL_LINE_SMOOTH);
		}
	}

	ImGui::End();
}


void ModuleImGUI::DrawHierarchyTab()
{
	ImGui::Begin("Hierarchy Tab", &open_tabs[GRAPHIC]);
	ImGui::Text("Use this tab to set the hierarchy of the scene objects");
	int id = 0;

	for (std::list<GameObject*>::iterator it = App->scene_intro->game_objects.begin(); it != App->scene_intro->game_objects.end(); it++)
		DrawHierarchyNode(*it, id);

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
			ComponentMesh* mesh = (ComponentMesh*)component;
			static bool wireframe_enabled;
			static bool mesh_active;

			wireframe_enabled = mesh->getWireframe();
			mesh_active = mesh->isActive();

			if (ImGui::Checkbox("Is active", &mesh_active))
				mesh->setActive(mesh_active);

			if (mesh_active)
			{

				if (ImGui::Checkbox("Wireframe", &wireframe_enabled))
					mesh->setWireframe(wireframe_enabled);

				if (ImGui::Button("Load checkered texture"))
					mesh->assignCheckeredMat();

				static char texture_name_buffer[64];
				ImGui::InputText("texture to load", texture_name_buffer, 64);

				ImGui::SameLine();
				if (ImGui::Button("Load Texture"))
					mesh->setMaterial(App->importer->quickLoadTex(texture_name_buffer));

				static char rootmesh_name_buffer[64];
				ImGui::InputText("root mesh to load", rootmesh_name_buffer, 64);

				ImGui::SameLine();
				if (ImGui::Button("Load Root Mesh"))
					App->importer->LoadRootMesh(rootmesh_name_buffer, mesh);

				if (ImGui::CollapsingHeader("Mesh Data"))
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

void ModuleImGUI::DrawPrimitivesTab()
{
	ImGui::Begin("Primitives", &open_tabs[PRIMITIVE]);
	ImGui::Text("Use this tab to add primitives to the scene");

	if (ImGui::Button("Add cube"))
	{
		GameObject* cube = new GameObject("Cube");
		cube->addComponent(new ComponentMesh(cube, Primitive_Cube));
		App->scene_intro->game_objects.push_back(cube);
	}
	if (ImGui::Button("Add plane"))
	{
		GameObject* plane = new GameObject("Plane");
		plane->addComponent(new ComponentMesh(plane, Primitive_Plane));
		App->scene_intro->game_objects.push_back(plane);
	}

	ImGui::End();
}

void ModuleImGUI::DrawImporterTab()
{

	ImGui::Begin("Importer", &open_tabs[PRIMITIVE]);
	ImGui::Text("Use this tab to fbx and other files into the scene");

	static char fbx_name_buffer[64];
	ImGui::InputText("FBX to load", fbx_name_buffer, 64);

	ImGui::SameLine();
	if (ImGui::Button("Load FBX"))
		App->scene_intro->game_objects.push_back(App->importer->LoadFBX(fbx_name_buffer));

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
	SDL_VERSION(&compiled);
	ImGui::Text("SDL %i", compiled.major);
	ImGui::SameLine();
	if (ImGui::Button("Learn more##sdl"))
		App->requestBrowser("https://wiki.libsdl.org/FrontPage");
	ImGui::Text("OpenGL %s", glGetString(GL_VERSION));
	ImGui::Text("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::SameLine();
	if (ImGui::Button("Learn more##opngl"))
		App->requestBrowser("https://www.opengl.org/");
	
	ImGui::End();
}

void ModuleImGUI::DrawWindowConfig() {
	static float brightnes = App->window->window_config.brightness;
	static int width = App->window->window_config.width;
	static int height = App->window->window_config.height;
	static bool fullscreen = App->window->window_config.fullscreen;
	static bool resizable = App->window->window_config.resizable;
	static bool borderless = App->window->window_config.borderless;
	static bool fulldesktop = App->window->window_config.fulldesk;
	ImGui::Begin("Window", &open_tabs[WINDOW_CONFIG]);
	ImGui::SliderFloat("Brightness", &brightnes, 0, 1.0f);
	ImGui::SliderInt("Width", &width, 0, 10000);
	ImGui::SliderInt("Height", &height, 0, 10000);
	// Update values
	App->window->setBrightness(brightnes);
	App->window->setSize(width, height);

	// Refresh rate
	ImGui::Text("Refresh Rate %i", (int)ImGui::GetIO().Framerate);

	//Bools
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
		App->window->setFullscreen(fullscreen);
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &resizable))
		App->window->setResizable(resizable);
	if (ImGui::Checkbox("Borderless", &borderless))
		App->window->setBorderless(!borderless);
	ImGui::SameLine();
	if (ImGui::Checkbox("FullDesktop", &fulldesktop))
		App->window->setFullDesktop(fulldesktop);


	ImGui::End();

}

void ModuleImGUI::DrawHardware() {
	ImGui::Begin("Hardware", &open_tabs[HARDWARE]);
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

	ImGui::End();
}

void ModuleImGUI::DrawApplication(){
	ImGui::Begin("Application", &open_tabs[APPLICATION]);
	// HARDCODED (?)
	ImGui::Text("App name: Kuroko Engine");
	ImGui::Text("Organization: UPC CITM");
	char title[25];
	sprintf_s(title, 25, "Framerate %.1f", App->fps_log[App->fps_log.size() - 1]);
	ImGui::PlotHistogram("##framerate", &App->fps_log[0], App->fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	sprintf_s(title, 25, "Milliseconds %.1f", App->ms_log[App->ms_log.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &App->ms_log[0], App->ms_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	ImGui::End();
}
void ModuleImGUI::SaveConfig(JSON_Object* config) {
	json_object_set_boolean(config, "demo", open_tabs[DEMO]);
	json_object_set_boolean(config, "graphic", open_tabs[GRAPHIC]);
	json_object_set_boolean(config, "test", open_tabs[TEST]);
	json_object_set_boolean(config, "hierarchy", open_tabs[HIERARCHY]);
	json_object_set_boolean(config, "obj_inspector", open_tabs[OBJ_INSPECTOR]);
	json_object_set_boolean(config, "primitive", open_tabs[PRIMITIVE]);
	json_object_set_boolean(config, "importer", open_tabs[IMPORTER]);
	json_object_set_boolean(config, "about", open_tabs[ABOUT]);
	json_object_set_boolean(config, "window_config", open_tabs[WINDOW_CONFIG]);
	json_object_set_boolean(config, "hardware", open_tabs[HARDWARE]);
	json_object_set_boolean(config, "application", open_tabs[APPLICATION]);
}
void ModuleImGUI::LoadConfig(JSON_Object* config) {
	open_tabs[DEMO] = json_object_get_boolean(config, "demo");
	open_tabs[GRAPHIC] = json_object_get_boolean(config, "graphic");
	open_tabs[TEST] = json_object_get_boolean(config, "test");
	open_tabs[HIERARCHY] = json_object_get_boolean(config, "hierarchy");
	open_tabs[OBJ_INSPECTOR] = json_object_get_boolean(config, "obj_inspector");
	open_tabs[PRIMITIVE] = json_object_get_boolean(config, "primitive");
	open_tabs[IMPORTER] = json_object_get_boolean(config, "importer");
	open_tabs[WINDOW_CONFIG] = json_object_get_boolean(config, "window_config");
	open_tabs[HARDWARE] = json_object_get_boolean(config, "hardware");
	open_tabs[APPLICATION] = json_object_get_boolean(config, "application");
	open_tabs[ABOUT] = json_object_get_boolean(config, "about");
}