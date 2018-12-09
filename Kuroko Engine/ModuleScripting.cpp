#include "ModuleScripting.h"
#include "Wren/wren.hpp"
#include "Applog.h"
#include "ModuleInput.h"

void ConsoleLog(WrenVM* vm); 
WrenForeignMethodFn bindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature); // Wren foraign methods
WrenForeignClassMethods bindForeignClass(WrenVM* vm, const char* module, const char* className);
char* loadModule(WrenVM* vm, const char* name);
void error(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message);
void write(WrenVM* vm, const char* text);

bool ModuleScripting::Init(const JSON_Object* config) 
{
	WrenConfiguration wconfig;
	wrenInitConfiguration(&wconfig);

	wconfig.bindForeignClassFn = bindForeignClass;
	wconfig.bindForeignMethodFn = bindForeignMethod;
	wconfig.writeFn = write;
	wconfig.errorFn = error;

	if (vm = wrenNewVM(&wconfig))
	{
		// Create base signature handles to call in every existing class
		base_signatures.insert(std::make_pair(std::string("Start()"), wrenMakeCallHandle(vm, "Start()")));
		base_signatures.insert(std::make_pair(std::string("Update()"), wrenMakeCallHandle(vm, "Update()")));
		base_signatures.insert(std::make_pair(std::string("new()"), wrenMakeCallHandle(vm, "new()")));
		// [...]

		return true;
	}
	else
		return false;
}

update_status ModuleScripting::Update(float dt) 
{
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
	{
		for (int i = 0; i < loaded_scripts.size(); i++)
		{
			wrenEnsureSlots(vm, 1);
			wrenSetSlotHandle(vm, 0, loaded_scripts[i]->class_handle);
			wrenCall(vm, base_signatures.at("Update()"));
		}
	}
	return UPDATE_CONTINUE;
}

bool ModuleScripting::CleanUp() 
{
	for(auto it = base_signatures.begin(); it != base_signatures.end(); it++)
		wrenReleaseHandle(vm, (*it).second);

	for (int i = 0; i < loaded_scripts.size(); i++)
		delete loaded_scripts[i];

	wrenFreeVM(vm);

	return true;
}


ScriptData* ModuleScripting::GenerateScript(const char* file_string_c, const char* file_name_c)
{
	std::string file_string = file_string_c;
	std::string file_name = file_name_c;

	if (CompileIntoVM(file_name.c_str(), file_string.c_str())) 
	{
		ScriptData* script = new ScriptData();
		script->class_name = file_name;
		script->class_handle = GetHandlerToClass(file_name.c_str(), file_name.c_str());
		
		std::vector<std::string> methods = GetMethodsFromClassHandler(script->class_handle);
		for (int i = 0; i < methods.size(); i++)
		{
			std::string method_name = methods[i];
			if (method_name.find("(") != std::string::npos)			// setter or method
			{
				if (method_name.find("=") == std::string::npos)		// method
				{
					std::string name;
					ImportedVariable::WrenDataType return_type_test = ImportedVariable::WrenDataType::WREN_NUMBER;
					std::string var_name_test = "test";
					std::vector<ImportedVariable> args;
					float var_value_test = 0.0f;

					size_t offset = method_name.find_first_of("(");
					for (auto it = method_name.find_first_of("_", offset + 1); it != std::string::npos; it = method_name.find_first_of("_", offset + 1))
					{
						offset = it;
						args.push_back(ImportedVariable(var_name_test.c_str(), return_type_test, (void*)&var_value_test));
					}

					for (int i = 0; i < method_name.find_first_of("("); i++)
						name.push_back(method_name.c_str()[i]);

					script->methods.push_back(ImportedMethod(name, return_type_test, args));
				}													// setters are ignored
			}						
			else													// var
			{
				float value_test = 0.0f;
				ImportedVariable var(method_name.c_str(), ImportedVariable::WrenDataType::WREN_NUMBER, &value_test);
				script->vars.push_back(var);
			}
		}

		loaded_scripts.push_back(script);
		return script;
	}
	else
		return nullptr;
}

bool ModuleScripting::CompileIntoVM(const char* module, const char* code) 
{
	return (wrenInterpret(vm, module, code) == WREN_RESULT_SUCCESS);
}

WrenHandle* ModuleScripting::GetHandlerToClass(const char* module, const char* class_name) 
{
	// Get static class
	wrenEnsureSlots(vm, 1);
	wrenGetVariable(vm, module, class_name, 0); // Class is now on slot 0
	

	// Call constructor to get instance
	wrenCall(vm, base_signatures.at("new()")); // instance of the class is now at slot 0
	
	// Get the prize
	return wrenGetSlotHandle(vm, 0);;
}

std::vector<std::string> ModuleScripting::GetMethodsFromClassHandler(WrenHandle * wrenClass) {

	if (!IS_OBJ(wrenClass->value)) {
		app_log->AddLog("Trying to get methods from a non object handler");
		return std::vector<std::string>();
	}

	std::vector<std::string> ret;
	Obj* cls = AS_OBJ(wrenClass->value);

	for (int i = 0; i < cls->classObj->methods.count; ++i) 
	{
		Method& method = cls->classObj->methods.data[i];

		if (method.type != METHOD_PRIMITIVE && method.type != METHOD_FOREIGN && method.type != METHOD_NONE ) 
			ret.push_back(method.as.closure->fn->debug->name);
	}

	return ret;
}


// Wren callbacks  ======================================================================

void write(WrenVM* vm, const char* text)
{
	app_log->AddLog(text);
}

void error(WrenVM* vm, WrenErrorType type, const char* module, int line, const char* message)
{
	if (type == WrenErrorType::WREN_ERROR_COMPILE)
		app_log->AddLog("Couldn't compile %s. %s error in %i line", module, message, line);
	else if (type == WrenErrorType::WREN_ERROR_RUNTIME)
		app_log->AddLog("Error when running %s. %s error in %i line", module, message, line);
}

char* loadModule(WrenVM* vm, const char* name)
{
	return nullptr;	// #import in wren, not used for now...
}

WrenForeignClassMethods bindForeignClass(WrenVM* vm, const char* module, const char* className)
{
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

WrenForeignMethodFn bindForeignMethod(WrenVM* vm, const char* module, const char* className, bool isStatic, const char* signature) // Wren foraign methods
{
	if (strcmp(module, "console_test") == 0)
	{
		if (strcmp(className, "EngineComunicator") == 0)
		{
			if (isStatic && strcmp(signature, "consoleOutput(_)") == 0)
				return ConsoleLog; // C function for EngineComunicator.consoleOutput

			if (!isStatic && strcmp(signature, "consoleOutput(_)") == 0)
				return ConsoleLog;

			// Other foreign methods on Math...
		}
		// Other classes in main...
	}
	// Other modules...
}

// C++ functions to be called from wren  ==================================================

void ConsoleLog(WrenVM* vm)
{
	static int call_times = 1;
	const char* message = wrenGetSlotString(vm, 1);

	app_log->AddLog("This is a C function called from wren, from a wren function called from C\nIt has been called %i times now\nIt contained this message: %s", call_times, message);
	call_times++;
}


// Helper class functions =================================================================

ImportedVariable::ImportedVariable(const char* name, WrenDataType type, void* _value)
{
	var_name = name;
	data_type = type;

	bool* var_bool = nullptr;
	float* var_float = nullptr;

	switch (type)
	{
	case WREN_BOOL: 
		var_bool = (bool*)_value;
		value.value_bool = *var_bool;
		break;
	case WREN_NUMBER:
		var_float = (float*)_value;
		value.value_number = *var_float;
		break;
	case WREN_STRING:
		value.value_string = (char*)_value;
		break;
	}
}

ImportedMethod::ImportedMethod(std::string name, ImportedVariable::WrenDataType ret_type, std::vector<ImportedVariable> args)
{
	method_name = name;
	return_type = ret_type;
	arg_list = args;
}




