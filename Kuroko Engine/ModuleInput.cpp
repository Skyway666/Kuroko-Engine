#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleImporter.h"
#include "ModuleRenderer3D.h"
#include "ModuleResourcesManager.h"
#include "FileSystem.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"

#include "Applog.h"

#define MAX_KEYS 300


Controller::Controller(SDL_GameController * controller, SDL_Haptic * controller_haptic) : controller(controller), controller_haptic(controller_haptic) {
	memset(axes, 0.f, sizeof(float) * SDL_CONTROLLER_AXIS_MAX);
	memset(buttons, KEY_IDLE, sizeof(KEY_STATE) * SDL_CONTROLLER_BUTTON_MAX);

	id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));

	if (SDL_HapticRumbleSupported(controller_haptic))
		SDL_HapticRumbleInit(controller_haptic);
}

Controller::~Controller()
{
}

bool Controller::isPressed(CONTROLLER_BUTTON button, KEY_STATE state) const
{
	return buttons[button] == state;;
}


ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	name = "input";
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init(const JSON_Object* config)
{
	app_log->AddLog("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		app_log->AddLog("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		switch (e.type)
		{
		case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

		case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				App->renderer3D->OnResize(e.window.data1, e.window.data2);

			break;

		case SDL_DROPFILE:
		{
			std::string extension = e.drop.file;
			App->fs.getExtension(extension);
			App->fs.copyFileTo(e.drop.file, ASSETS, extension.c_str());
			app_log->AddLog("%s copied to Assets folder", e.drop.file);
		}

		case SDL_CONTROLLERDEVICEADDED:
			if (SDL_IsGameController(e.cdevice.which)) {
				int index = e.cdevice.which;
				Controller* ctrl = new Controller(SDL_GameControllerOpen(index), SDL_HapticOpen(index));
				if (ctrl != nullptr)
					controllers.push_back(ctrl);
			}
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
			for (std::list<Controller*>::iterator it = controllers.begin(); it != controllers.end(); ++it) {
				if ((*it)->getControllerID() == e.cdevice.which) {
					delete (*it);
					it = controllers.erase(it);
					break;
				}
			}
		case SDL_CONTROLLERBUTTONDOWN:
			for (std::list<Controller*>::iterator it = controllers.begin(); it != controllers.end(); ++it) {
				if ((*it)->getControllerID() == e.cbutton.which) {
					if ((*it)->buttons[e.cbutton.button] != KEY_REPEAT || (*it)->buttons[e.cbutton.button] != KEY_DOWN) {
						(*it)->buttons[e.cbutton.button] = KEY_DOWN;
						break;
					}
				}
			}
				break;

		case SDL_CONTROLLERBUTTONUP:
			for (std::list<Controller*>::iterator it = controllers.begin(); it != controllers.end(); ++it) {
				if ((*it)->getControllerID() == e.cbutton.which) {
					(*it)->buttons[e.cbutton.button] = KEY_UP;
					break;
				}
			}
			break;
		default: break;
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	app_log->AddLog("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}


