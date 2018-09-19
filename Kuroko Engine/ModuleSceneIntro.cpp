#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "ModuleCamera3D.h"
#include "imgui.h"

#include "GameObject.h"			// <--  testing purposes
#include "ModuleImporter.h"	// <--  testing purposes


#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */


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

	return UPDATE_CONTINUE;
}




