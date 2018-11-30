#pragma once
#include "Module.h"
#include "Application.h"


struct WrenVM;
class ModuleScripting : public Module {
public:
	ModuleScripting(Application* app, bool start_enabled = true);
	~ModuleScripting();

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();


private:

	WrenVM* vm;
};

