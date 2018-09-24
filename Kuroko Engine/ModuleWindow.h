#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

struct window_config {
	int width, height;
	float brightness;
	bool resizable, fullscreen, borderless, fulldesk;
};
class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(JSON_Object* config);
	bool CleanUp();

	void SetTitle(const char* title);
	void setResizable(bool resize);
	void setFullscreen(bool fullscreen);
	void setBorderless(bool borderless);
	void setFullDesktop(bool fulldesk);
	void setBrightness(float brightness);
	void setSetSize(int x, int y);

	void fillWindowConfig(JSON_Object* config);

	void SaveConfig(JSON_Object* config);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	window_config window_config;

	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__