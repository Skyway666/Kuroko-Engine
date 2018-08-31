#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer2D.h"
#include "ModuleInput.h"
#include "imgui.h"


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

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}



