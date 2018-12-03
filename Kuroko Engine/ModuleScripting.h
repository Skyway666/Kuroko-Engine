#pragma once
#include "Module.h"
#include "Application.h"


struct WrenVM;
struct WrenHandle;
class ModuleScripting : public Module {
public:
	ModuleScripting(Application* app, bool start_enabled = true);
	~ModuleScripting();

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();


	bool CompileIntoVM(const char* module, const char* code);
	WrenHandle* GetHandlerToClass(const char* module, const char* class_name);
	WrenVM* vm;
private:
	// Just for testing
	WrenHandle* update_signature;
	WrenHandle* test_script_class;

};

