#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

#include <list>

class Application;

struct Window
{
	SDL_Window* window = nullptr;
	SDL_Surface* screen_surface = nullptr;

	int width = 0; int height = 0;
	float brightness = 1.0f;
	bool resizable = false; bool fullscreen = false; bool borderless = false; bool fulldesk = false;
	uint id = 0;
};


class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(JSON_Object* config);
	bool CleanUp();

	// id = 0 assigns to main window
	void SetTitle(const char* title, uint id = 0);
	void setResizable(bool resize, uint id = 0);
	void setFullscreen(bool fullscreen, uint id = 0);
	void setBorderless(bool borderless, uint id = 0);
	void setFullDesktop(bool fulldesk, uint id = 0);
	void setBrightness(float brightness, uint id = 0);
	void setSize(int x, int y, uint id = 0);

	void fillWindowConfig(JSON_Object* config);

	void SaveConfig(JSON_Object* config) const;
	void LoadConfig(JSON_Object* config);

public:

	Window* main_window = nullptr;
	std::list<Window*> aux_windows;
};

#endif // __ModuleWindow_H__