#include "ResourceScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleScripting.h"
#include "ScriptData.h"


ResourceScript::ResourceScript(resource_deff deff): Resource(deff)  {

	// Get the binary
	JSON_Value* script_binary = json_parse_file(deff.binary.c_str());

	// Read code and class name
	code = json_object_get_string(json_object(script_binary), "code");
	class_name = json_object_get_string(json_object(script_binary), "class_name");
	json_value_free(script_binary);
}

void ResourceScript::Compile() {
	App->scripting->CompileIntoVM(class_name.c_str(), code.c_str()); // If return false invalidate resource
}

void ResourceScript::GenerateScript() {
	data = App->scripting->GenerateScript(class_name.c_str());
}

ResourceScript::~ResourceScript() {
	wrenReleaseHandle(App->scripting->vm, data->class_handle);
	delete data;
}


