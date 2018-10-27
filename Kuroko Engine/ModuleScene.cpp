#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera3D.h"
#include "ImGui\imgui.h"
#include "Material.h"
#include "Mesh.h"
#include "ModuleInput.h"
#include "GameObject.h"		
#include "Camera.h"
#include "Skybox.h"
#include "Quadtree.h"

#include "glmath.h"
#include "ModuleImporter.h" // TODO: remove this include and set skybox creation in another module (Importer?, delayed until user input?)

#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#include <array>

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "scene";
}

ModuleScene::~ModuleScene(){}

// Load assets
bool ModuleScene::Start()
{
	skybox = new Skybox();

	std::array<Texture*, 6> skybox_texs;
	skybox_texs[LEFT]	= (Texture*)App->importer->Import("skybox_default_left.png", I_TEXTURE);
	skybox_texs[RIGHT]	= (Texture*)App->importer->Import("skybox_default_right.png", I_TEXTURE);
	skybox_texs[UP]		= (Texture*)App->importer->Import("skybox_default_up.png", I_TEXTURE);
	skybox_texs[DOWN]	= (Texture*)App->importer->Import("skybox_default_down.png", I_TEXTURE);
	skybox_texs[FRONT]	= (Texture*)App->importer->Import("skybox_default_front.png", I_TEXTURE);
	skybox_texs[BACK]	= (Texture*)App->importer->Import("skybox_default_back.png", I_TEXTURE);
	skybox->setAllTextures(skybox_texs);

	// TEST FOR QUADTREE
	quadtree = new Quadtree(AABB(float3(-50, -10, -50), float3(50, 10, 50)));
	// TEST FOR QUADTREE
	return true;
}

// Load assets
bool ModuleScene::CleanUp()
{
	for (auto it = game_objects.begin(); it != game_objects.end();it++)
		delete *it;

	for (auto it = materials.begin(); it != materials.end(); it++)
		delete *it;

	for (auto it = meshes.begin(); it != meshes.end(); it++)
		delete *it;

	for (auto it = textures.begin(); it != textures.end(); it++)
		delete *it;

	game_objects.clear();
	materials.clear();
	meshes.clear();
	textures.clear();

	game_objs_to_delete.clear();
	materials_to_delete.clear();
	meshes_to_delete.clear();
	textures_to_delete.clear();

	selected_obj = nullptr;
	return true;
}

update_status ModuleScene::PostUpdate(float dt)
{
	// TEST FOR QUADTREE
	quadtree->DebugDraw();

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) {
		for (auto it = game_objects.begin(); it != game_objects.end(); it++)
			quadtree->Insert(*it);
	}
	// TEST FOR QUADTREE
	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
	{
		if (*it == selected_obj) selected_obj = nullptr;
		game_objects.remove(*it);

		if (GameObject* parent = (*it)->getParent())
			parent->removeChild(*it);

		delete *it;
	}

	for (auto it = materials_to_delete.begin(); it != materials_to_delete.end(); it++)
	{
		materials.remove(*it);
		delete *it;
	}

	for (auto it = textures_to_delete.begin(); it != textures_to_delete.end(); it++)
	{
		textures.remove(*it);
		delete *it;
	}

	for (auto it = meshes_to_delete.begin(); it != meshes_to_delete.end(); it++)
	{
		meshes.remove(*it);
		delete *it;
	}

	game_objs_to_delete.clear();
	materials_to_delete.clear();
	textures_to_delete.clear();
	meshes_to_delete.clear();

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleScene::Update(float dt)
{
	if (skybox)
	{
		skybox->updatePosition(float3(App->camera->editor_camera->Position.x , App->camera->editor_camera->Position.y, App->camera->editor_camera->Position.z));
		skybox->Draw();
	}

	if (draw_grid)	DrawGrid();

	std::list<GameObject*> root_objs;
	getRootObjs(root_objs);

	for (auto it = root_objs.begin(); it != root_objs.end(); it++)
		(*it)->Update(dt);


	return UPDATE_CONTINUE;
}

Material* ModuleScene::getMaterial(uint id)  const
{
	for (auto it = materials_to_delete.begin(); it != materials_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = materials.begin(); it != materials.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}

GameObject* ModuleScene::getGameObject(uint id) const
{
	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
		if (id == (*it)->getId())
			return *it;

	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}
Mesh* ModuleScene::getMesh(uint id) const
{
	for (auto it = meshes_to_delete.begin(); it != meshes_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = meshes.begin(); it != meshes.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}
Texture* ModuleScene::getTexture(uint id) const
{
	for (auto it = textures_to_delete.begin(); it != textures_to_delete.end(); it++)
		if (id == (*it)->getId())
			return nullptr;

	for (auto it = textures.begin(); it != textures.end(); it++)
		if (id == (*it)->getId())
			return *it;

	return nullptr;
}

void ModuleScene::getRootObjs(std::list<GameObject*>& list_to_fill)
{
	for (auto it = game_objects.begin(); it != game_objects.end(); it++)
		if (!(*it)->getParent())
			list_to_fill.push_back(*it);

	for (auto it = game_objs_to_delete.begin(); it != game_objs_to_delete.end(); it++)
		for (auto it2 = list_to_fill.begin(); it2 != list_to_fill.end(); it2++)
			if (*it == *it2)
				list_to_fill.remove(*it);
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



