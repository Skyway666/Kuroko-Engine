#include "ModuleScripting.h"
#include "Wren/wren.hpp"
#include "Applog.h"
#include "ModuleInput.h"

// C++ functions to be called from wren
void ConsoleLog(WrenVM* vm) {
	const char* message = wrenGetSlotString(vm, 1);

	static int call_times = 1;
	app_log->AddLog("This is a C function called from wren, from a wren function called from C\nIt has been called %i times now\nIt contained this message:", call_times);
	app_log->AddLog(message);
	call_times++;
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
			if (!isStatic && strcmp(signature, "consoleOutput(_)") == 0) {
				return ConsoleLog;
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

	CompileIntoVM("console_test", wren_content.c_str()); // Compile the code


	update_signature = wrenMakeCallHandle(vm, "Update()"); // Create "Update()" signature handle to call it in every existing class


	test_script_class = GetHandlerToClass("console_test", "TestScript");

	GetMethodsFromClassHandler(test_script_class);


	return true;
}

update_status ModuleScripting::Update(float dt) {

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
		wrenSetSlotHandle(vm, 0, test_script_class);
		wrenCall(vm, update_signature);
	}
	return UPDATE_CONTINUE;
}

bool ModuleScripting::CleanUp() {

	wrenReleaseHandle(vm, update_signature);
	wrenReleaseHandle(vm, test_script_class);
	wrenFreeVM(vm);
	return true;
}

bool ModuleScripting::CompileIntoVM(const char* module, const char* code) {
	WrenInterpretResult result = wrenInterpret(vm, module, code);

	bool ret = false;

	if (result == WREN_RESULT_SUCCESS)
		ret = true;

	return ret;
}

WrenHandle* ModuleScripting::GetHandlerToClass(const char* module, const char* class_name) {
	wrenEnsureSlots(vm, 1);
	wrenGetVariable(vm, module, class_name, 0);

	return wrenGetSlotHandle(vm, 0);
}

std::vector<std::string> ModuleScripting::GetMethodsFromClassHandler(WrenHandle * wrenClass) {

	if (!IS_CLASS(wrenClass->value)) {
		app_log->AddLog("Trying to get methods from a non class handler");
		return std::vector<std::string>();
	}

	std::vector<std::string> ret;
	ObjClass* cls = AS_CLASS(wrenClass->value);
	app_log->AddLog("%.*s\n", cls->name->length, cls->name->value);
	for (int i = 0; i < cls->methods.count; ++i) {
		Method& method = cls->methods.data[i];
		if (method.type != METHOD_PRIMITIVE && method.type != METHOD_NONE) {
			ret.push_back(method.as.closure->fn->debug->name);
		}
	}

	return ret;

}






