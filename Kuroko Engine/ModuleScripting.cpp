#include "ModuleScripting.h"
#include "Wren/wren.hpp"


ModuleScripting::ModuleScripting(Application* app, bool start_enabled): Module(app, start_enabled) {
	name = "scripting";
}


ModuleScripting::~ModuleScripting() {
}

bool ModuleScripting::Init(const JSON_Object* config) {
	WrenConfiguration wconfig;
	wrenInitConfiguration(&wconfig);

	vm = wrenNewVM(&wconfig);

	return true;
}

update_status ModuleScripting::Update(float dt) {

	return UPDATE_CONTINUE;
}

bool ModuleScripting::CleanUp() {

	wrenFreeVM(vm);
	return true;
}
