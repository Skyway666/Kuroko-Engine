#include "ResourceScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleScripting.h"


ResourceScript::ResourceScript(resource_deff deff): Resource(deff)  {

	// Get the binary
	JSON_Value* script_binary = json_parse_file(deff.binary.c_str());

	// Read code and class name
	std::string code = json_object_get_string(json_object(script_binary), "code");
	std::string class_name = json_object_get_string(json_object(script_binary), "class_name");



	// Compile it into VM and store everything needed
	data = App->scripting->GenerateScript(code.c_str(), class_name.c_str());
	json_value_free(script_binary);
}

ResourceScript::~ResourceScript() {
	wrenReleaseHandle(App->scripting->vm, data->class_handle);
	delete data;
}


