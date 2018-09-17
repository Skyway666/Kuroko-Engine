#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleCamera3D.h"
#include "imgui.h"

#include "GameObject.h"			// <--  testing purposes
#include "ModuleImporter.h"	// <--  testing purposes

#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#pragma comment( lib, "glew-2.1.0/lib/glew32.lib")
#pragma comment( lib, "glew-2.1.0/lib/glew32s.lib")

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
}

// Load assets
bool ModuleScene::Start()
{
	game_objects.push_back(App->importer->LoadFBX("BakerHouse.fbx"));
	return true;
}

// Load assets
bool ModuleScene::CleanUp(){

	game_objects.clear();
	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	Primitives::Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	for (std::list<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); it++)
		(*it)->Update(dt);

	bool close_app = false;
	if (show_engine_about)
		ShowAboutWindow();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Quit"))
				close_app = true;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About", NULL, &show_engine_about);
			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	if (!close_app)
		return UPDATE_CONTINUE;
	else
		return UPDATE_STOP;
}

void ModuleScene::ShowAboutWindow() {
	ImGui::Begin("About", &show_engine_about);
	ImGui::Text("Kuroko Engine");
	ImGui::Separator();
	ImGui::Text("By Rodrigo (no me se tu nombre completo) and Lucas Garcia Mateu.");
	ImGui::Text("Kuroko Engine is licensed under the MIT License.\n");
	ImGui::Text("Using Glew %s", glewGetString(GLEW_VERSION));
	ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
	ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
	ImGui::Text("OpenGL version supported %s", glGetString(GL_VERSION));
	ImGui::Text("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::End();
}


