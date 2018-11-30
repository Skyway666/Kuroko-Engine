#include "ModuleScripting.h"
#include "Wren/wren.hpp"
#include "Applog.h"

// C++ functions to be called from wren
void ConsoleLog(WrenVM* vm) {
	const char* message = wrenGetSlotString(vm, 1);
	app_log->AddLog(message);
}

void write(WrenVM* vm, const char* text) {
	app_log->AddLog(text);
}
void error(WrenVM* vm,WrenErrorType type, const char* module, int line, const char* message) {
	if (type == WrenErrorType::WREN_ERROR_COMPILE) {
		app_log->AddLog("Couldn't compile %s. %s error in %i line", module, message, line);
	}
	if (type == WrenErrorType::WREN_ERROR_RUNTIME) {
		app_log->AddLog("Error when running %s. %s error in %i line", module, message, line);
	}
}

// Wren callbacks
char* loadModule(WrenVM* vm, const char* name) { // #import in wren, not used for now...
	return nullptr;
}
WrenForeignClassMethods bindForeignClass(WrenVM* vm, const char* module, const char* className) {

	WrenForeignClassMethods methods;

	//if (strcmp(className, "File") == 0) {
	//	methods->allocate = fileAllocate;
	//	methods->finalize = fileFinalize;
	//}
	//else {
	//	// Unknown class.
	methods.allocate = NULL;
	methods.finalize = NULL;
	//}

	return methods;
}
WrenForeignMethodFn bindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature) { // Wren foraign methods
	if (strcmp(module, "console_test") == 0) {
		if (strcmp(className, "EngineComunicator") == 0) {
			if (isStatic && strcmp(signature, "consoleOutput(_)") == 0) {
				return ConsoleLog; // C function for EngineComunicator.consoleOutput
			}
			// Other foreign methods on Math...
		}
		// Other classes in main...
	}
	// Other modules...
}

ModuleScripting::ModuleScripting(Application* app, bool start_enabled): Module(app, start_enabled) {
	name = "scripting";
}


ModuleScripting::~ModuleScripting() {
}

bool ModuleScripting::Init(const JSON_Object* config) {
	WrenConfiguration wconfig;
	wrenInitConfiguration(&wconfig);


	wconfig.bindForeignClassFn = bindForeignClass;
	wconfig.bindForeignMethodFn = bindForeignMethod;
	wconfig.writeFn = write;
	wconfig.errorFn = error;


	vm = wrenNewVM(&wconfig);
	std::string wren_content = App->fs.GetFileString("Assets/Scripts/console_test.wren");
	WrenInterpretResult result = wrenInterpret(vm, "console_test", wren_content.c_str());

	return true;
}

update_status ModuleScripting::Update(float dt) {

	return UPDATE_CONTINUE;
}

bool ModuleScripting::CleanUp() {

	wrenFreeVM(vm);
	return true;
}








