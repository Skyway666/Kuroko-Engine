#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ImGui\imgui.h"
#include "Material.h"
#include "ModuleInput.h"

#include "GameObject.h"			// <--  testing purposes
#include "ModuleImporter.h"	// <--  testing purposes
#include "ModuleDebug.h" // <--  testing purposes
#include "glmath.h"


#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene(){}

// Load assets
bool ModuleScene::Start()
{
	return true;
}

// Load assets
bool ModuleScene::CleanUp(){

	game_objects.clear();
	materials.clear();
	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{

	if (draw_grid)
		DrawGrid();

	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
	{
		if (selected_obj == *it) selected_obj = nullptr;
		game_objects.remove(*it);
	}

	for (std::list<GameObject*>::iterator it = game_objects.begin(); it != game_objects.end(); it++)
		(*it)->Update(dt);

	//Just for assigment one
	//game_objects.back()->Update(dt);

	return UPDATE_CONTINUE;
}

Material* ModuleScene::getMaterial(uint id)  const
{
	for (std::list<Material*>::const_iterator it = materials.begin(); it != materials.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}

void ModuleScene::DrawGrid() const
{
	glLineWidth(1.0f);
	glColor3f(0.5f, 0.5f, 0.5f);
	glBegin(GL_LINES);

	float d = 20.0f;

	for (float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();

}



