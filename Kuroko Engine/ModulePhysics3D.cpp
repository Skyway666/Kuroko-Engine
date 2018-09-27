#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "imgui.h"
#include "Applog.h"

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled) {
	debug = true;
	name = "physics";
}

// Destructor
ModulePhysics3D::~ModulePhysics3D() {
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init(JSON_Object* config) {
	app_log->AddLog("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start() {
	app_log->AddLog("Creating Physics environment");

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt) {

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt) {
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt) {
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp() {
	app_log->AddLog("Destroying 3D Physics simulation");

	return true;
}
