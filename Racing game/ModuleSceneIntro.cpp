#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleCamera3D.h"
#include "imgui.h"

#include "GameObject.h"			// <--  testing purposes
#include "ModuleFBXimporter.h"	// <--  testing purposes


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
}

// Load assets
bool ModuleSceneIntro::Start()
{
	test = App->fbx_importer->LoadFBX("BakerHouse.fbx");
	return true;
}

// Load assets
bool ModuleSceneIntro::CleanUp(){
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Primitives::Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	test->Update(dt);

	//Little menu to close app
	bool close_app = false;
	ImGui::Begin("Close app");

	if (ImGui::Button("Quit"))
		close_app = true;

	ImGui::End();


	if (!close_app)
		return UPDATE_CONTINUE;
	else
		return UPDATE_STOP;
}


