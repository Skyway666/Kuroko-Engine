#include "ModuleRenderer2D.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"



ModuleRenderer2D::ModuleRenderer2D(Application* app, bool start_enabled): Module(app, start_enabled) {
}


ModuleRenderer2D::~ModuleRenderer2D() {

}

bool ModuleRenderer2D::Init() {

	LOG("Creating Renderer context");
	bool ret = true;
	Uint32 flags = 0;

	if (VSYNC == true) {
		flags |= SDL_RENDERER_PRESENTVSYNC;
	}

	renderer = SDL_CreateRenderer(App->window->window, -1, flags);

	if (renderer == NULL) {
		LOG("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
	return true;
}

update_status ModuleRenderer2D::PreUpdate(float dt) {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	return UPDATE_CONTINUE;
}

update_status ModuleRenderer2D::Update(float dt) {
	int speed = 3;

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		camera.y += speed;

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		camera.y -= speed;

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		camera.x += speed;

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		camera.x -= speed;

	return UPDATE_CONTINUE;
}
update_status ModuleRenderer2D::PostUpdate(float dt) {
	SDL_RenderPresent(renderer);
	return UPDATE_CONTINUE;
}

bool ModuleRenderer2D::CleanUp() {
	LOG("Destroying renderer");

	//Destroy window
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}

	return true;
}

void ModuleRenderer2D::SetViewportSize(int width, int height) {
	SDL_Rect rect = { 0,0,width, height };

	SDL_RenderSetViewport(renderer, &rect);
}