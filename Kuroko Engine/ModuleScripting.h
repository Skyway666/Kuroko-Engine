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
	void ReleaseHandlerToClass(WrenHandle* handle);
	WrenHandle* getHandlerToMethod(const char* method_name);

	std::vector<std::string> GetMethodsFromClassHandler(WrenHandle* wrenClass);
	ScriptData* GenerateScript(const char* file_name);

public:

	WrenVM* vm							= nullptr;
	std::map<std::string, std::string> edited_scripts;
	std::map<std::string, WrenHandle*> base_signatures;
	std::list<ScriptData*> loaded_instances;

private:

	std::vector<ScriptData*> loaded_scripts;

};

#endif
