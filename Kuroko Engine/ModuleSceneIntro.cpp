#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "imgui.h"

#include "GameObject.h"			// <--  testing purposes
#include "ModuleImporter.h"	// <--  testing purposes
#include "glmath.h"


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
	Scentre = float3(0.0f, 0.0f, 4.0f);
	Sradius = 3.0f;
	sphere = Sphere(Scentre, Sradius);

	Ccentre = float3(0.0f, 0.0f, -4.0f);
	Csize = float3(4.0f, 4.0f, 4.0f);
	cube = AABB(Ccentre, Csize);
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
	/*Primitives::Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	ImGui::Begin("MatGeoLib testing");
	//Sphere
		ImGui::Text("Sphere Values");
		ImGui::Text("Position");
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("SPx", &Scentre.x, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("SPy", &Scentre.y, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("SPz", &Scentre.z,0.15f);
		ImGui::Text("Radius");
		ImGui::DragFloat("Radius", &Sradius);
		sphere.pos = Scentre;
		sphere.r = Sradius;
	//Cube
		ImGui::Text("Cube Values");
		ImGui::Text("Position");
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CPx", &Ccentre.x, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CPy", &Ccentre.y, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CPz", &Ccentre.z, 0.15f);
		ImGui::Text("Size");
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CSx", &Csize.x, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CSy", &Csize.y, 0.15f);
		ImGui::SameLine();
		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("CSz", &Csize.z, 0.15f);
		cube.SetFromCenterAndSize(Ccentre, Csize);

		if (cube.Intersects(sphere))
			ImGui::Text("Collision!");
	ImGui::End();


	for (std::list<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); it++)
		(*it)->Update(dt);

	return UPDATE_CONTINUE;
}




