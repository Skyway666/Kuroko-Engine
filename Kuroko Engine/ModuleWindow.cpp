#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Applog.h"

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
	app_log->AddLog("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		app_log->AddLog("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		fillWindowConfig(config);
		//Create window
		int width = window_config.width * SCREEN_SIZE; 
		int height = window_config.height * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(window_config.fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(window_config.resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(window_config.borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(window_config.fulldesk)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(json_object_get_string(config,"title"), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			app_log->AddLog("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}
	setBrightness(window_config.brightness);

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	app_log->AddLog("Destroying SDL window and quitting all SDL systems");

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

	SDL_SetWindowBordered(window, (SDL_bool)!borderless);
}

void ModuleWindow::setFullDesktop(bool fulldesktop) {
	Uint32 flag;
	if (fulldesktop)
		flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
	else
		flag = 0;

	SDL_SetWindowFullscreen(window, flag);
}

void ModuleWindow::setBrightness(float brightness) {

	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::setSize(int x, int y) {

	SDL_SetWindowSize(window, x, y);
}

void ModuleWindow::fillWindowConfig(JSON_Object* config) {
	window_config.borderless = json_object_get_boolean(config, "borderless");
	window_config.fullscreen = json_object_get_boolean(config, "fullscreen");
	window_config.resizable = json_object_get_boolean(config, "resizable");
	window_config.fulldesk = json_object_get_boolean(config, "fulldesktop");

	window_config.width = json_object_get_number(config, "width");
	window_config.height = json_object_get_number(config, "height");

	window_config.brightness = json_object_get_number(config, "brightness");
}

void ModuleWindow::SaveConfig(JSON_Object* config) {
	json_object_set_boolean(config, "borderless", window_config.borderless);
	json_object_set_boolean(config, "fullscreen", window_config.fullscreen);
	json_object_set_boolean(config, "resizable", window_config.resizable);
	json_object_set_boolean(config, "fulldesktop", window_config.fulldesk);

	json_object_set_number(config, "width", window_config.width);
	json_object_set_number(config, "height", window_config.height);
	json_object_set_number(config, "brightness", window_config.brightness);
}

void ModuleWindow::LoadConfig(JSON_Object* config) {

	fillWindowConfig(config);
	setFullscreen(json_object_get_boolean(config, "fullscreen"));
	setResizable(json_object_get_boolean(config, "resizable"));
	setFullDesktop(json_object_get_boolean(config, "fulldesktop"));
	setBorderless(json_object_get_boolean(config, "borderless"));

	setSize(json_object_get_number(config, "width"), json_object_get_number(config, "height"));

	setBrightness(json_object_get_number(config, "brightness"));
}
