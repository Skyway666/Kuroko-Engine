#ifndef __MODULE_SCRIPTING__
#define __MODULE_SCRIPTING__
#include "Module.h"
#include "Application.h"
#include <map>
#include <vector>

struct WrenVM;
struct WrenHandle;
struct ScriptData;


class ModuleScripting : public Module {
public:
	ModuleScripting(Application* app, bool start_enabled = true) : Module(app, start_enabled) { name = "scripting"; };
	~ModuleScripting() {};

	bool Init(const JSON_Object* config);
	update_status Update(float dt);
	bool CleanUp();

	bool CompileIntoVM(const char* module, const char* code);
	WrenHandle* GetHandlerToClass(const char* module, const char* class_name);

	std::vector<std::string> GetMethodsFromClassHandler(WrenHandle* wrenClass);
	ScriptData* GenerateScript(const char* file_path);

public:

	WrenVM* vm							= nullptr;
	std::map<std::string, std::string> edited_scripts;
private:

	std::map<std::string, WrenHandle*> base_signatures;
	std::vector<ScriptData*> loaded_scripts;

};





struct ImportedVariable
{
	enum WrenDataType { WREN_UNKNOWN, WREN_BOOL, WREN_STRING, WREN_NUMBER };

	ImportedVariable(const char* name, WrenDataType type, void* value);
private:
	union
	{
		bool value_bool = false;
		float value_number;
		const char* value_string;
	};

	std::string var_name;
	WrenDataType data_type = WREN_UNKNOWN;

public:
	void* GetValue();
	WrenDataType getType() { return data_type; }
	std::string getName() { return var_name; };

};


struct ImportedMethod
{
	ImportedMethod(std::string name, ImportedVariable::WrenDataType ret_type, std::vector<ImportedVariable> args);

private:
	std::vector<ImportedVariable> arg_list;
	std::string method_name;
	ImportedVariable::WrenDataType return_type = ImportedVariable::WrenDataType::WREN_UNKNOWN;

public:
	std::string getName() { return method_name; };
	ImportedVariable::WrenDataType getReturnType() { return return_type; }

	std::vector<ImportedVariable> getArgList() { return arg_list; };
};


struct ScriptData
{
	std::string full_path;
	std::string filename;

	WrenHandle* wren_handle = nullptr;

	std::vector<ImportedVariable> vars;
	std::vector<ImportedMethod>  methods;
};

#endif
