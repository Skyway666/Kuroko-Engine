#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"


ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	name = "window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(JSON_Object* config)
{
	APPLOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		APPLOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = json_object_get_number(json_array_get_object(json_object_get_array(config, "resolution"), 0),"value") * SCREEN_SIZE; // A bit of a hardcod, but nothing important
		int height = json_object_get_number(json_array_get_object(json_object_get_array(config, "resolution"), 1), "value") * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(json_object_get_boolean(config, "fullscreen"))
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(json_object_get_boolean(config, "resizable"))
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(json_object_get_boolean(config, "borderless"))
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(json_object_get_boolean(config, "fulldesktop"))
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(json_object_get_string(config,"title"), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			APPLOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	APPLOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::setResizable(bool resizable) {
	SDL_SetWindowResizable(window,(SDL_bool)resizable);
}
void ModuleWindow::setFullscreen(bool fullscreen) {
	Uint32 flag;
	if (fullscreen)
		flag = SDL_WINDOW_FULLSCREEN;
	else
		flag = 0;

	SDL_SetWindowFullscreen(window, flag);
}

void ModuleWindow::setBorderless(bool borderless) {
	SDL_SetWindowBordered(window, (SDL_bool)borderless);
}

void ModuleWindow::setFullDesktop(bool fulldesktop) {
	Uint32 flag;
	if (fulldesktop)
		flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		flag = 0;

	SDL_SetWindowFullscreen(window, flag);
}

void ModuleWindow::setBrightnes(float brightness) {
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::setSetSize(int x, int y) {
	SDL_SetWindowSize(window, x, y);
}
