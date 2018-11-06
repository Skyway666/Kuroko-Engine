#ifndef __MODULE_TIME_MANAGER
#define __MODULE_TIME_MANAGER

#include "Module.h"

class ModuleTimeManager: public Module
{
public:
	ModuleTimeManager(Application* app, bool start_enabled = true);
	~ModuleTimeManager();

	bool Init(const JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
};

#endif