#include "ResourceScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleScripting.h"


ResourceScript::ResourceScript(resource_deff deff): Resource(deff)  {
	std::string full_binary_path;
	App->fs.FormFullPath(full_binary_path, deff.binary.c_str(), LIBRARY_SCRIPTS, "");
	JSON_Value* script_binary = json_parse_file(full_binary_path.c_str());
	std::string code = json_object_get_string(json_object(script_binary), "code");
	std::string class_name = json_object_get_string(json_object(script_binary), "class_name");
	json_value_free(script_binary);

	App->scripting->CompileIntoVM(class_name.c_str(), code.c_str()); // If it can't compile, put this resource as invalid
	classHandle = App->scripting->GetHandlerToClass(class_name.c_str(), class_name.c_str()); // Store the handler so it can be called with any needed arguments
}


ResourceScript::~ResourceScript() {
	wrenReleaseHandle(App->scripting->vm, classHandle);
}
