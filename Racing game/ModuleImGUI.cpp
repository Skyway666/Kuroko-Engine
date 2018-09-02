#include "ModuleImGUI.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl2.h"
#include "GameObject.h"

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>



ModuleImGUI::ModuleImGUI(Application* app, bool start_enabled) : Module(app, start_enabled) {

}


ModuleImGUI::~ModuleImGUI() {
}

bool ModuleImGUI::Init() {
	
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

	show_demo_window = true;
	show_graphic_tab = true;
	show_test_tab = true;
	show_hierarchy_tab = true;

	return true;
}

update_status ModuleImGUI::PreUpdate(float dt) {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			return UPDATE_STOP;
	}

	// Start the ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

update_status ModuleImGUI::Update(float dt) {


	// 1. Show a simple window.
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
	if(show_test_tab)
	{
		// features functionality

		ImGui::Checkbox("Graphic tab", &show_graphic_tab);

		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state

		// test functionality
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    


		if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.

	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) 
		show_graphic_tab = !show_graphic_tab;

	if (show_graphic_tab) DrawGraphicsTab();

	// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
	if (show_demo_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	if (show_hierarchy_tab)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
		DrawHierarchyTab();
	}

	return UPDATE_CONTINUE;

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


	ImGui::Begin("Graphics Tab", &show_graphic_tab);
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
	ImGui::Begin("Hierarchy Tab", &show_graphic_tab);
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

