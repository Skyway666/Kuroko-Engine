#include "ScriptData.h"
#include "ModuleScripting.h"
#include "Application.h"

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

ImportedMethod::ImportedMethod(std::string name, ImportedVariable::WrenDataType ret_type, std::vector<ImportedVariable> args, WrenHandle* handle)
{
	method_name = name;
	return_type = ret_type;
	arg_list = args;
	method_handle = handle;
}


ScriptData::~ScriptData()
{
	class_name.clear();
	vars.clear();
	methods.clear();
	if (class_handle)
		App->scripting->ReleaseHandlerToClass(class_handle);
}


