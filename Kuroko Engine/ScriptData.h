#pragma once
#include <string>
#include <vector>

struct WrenHandle;

union Var {
	bool value_bool = false;
	float value_number;
	const char* value_string;
};

struct ImportedVariable
{
	enum WrenDataType { WREN_UNKNOWN, WREN_BOOL, WREN_STRING, WREN_NUMBER };

	ImportedVariable(const char* name, WrenDataType type, void* value);
private:
	Var value;
	std::string var_name;
	WrenDataType data_type = WREN_UNKNOWN;
	bool is_public = true;
	bool edited = false;

public:
	Var GetValue() { return value; }
	void SetValue(Var value) { this->value = value; }
	WrenDataType getType() { return data_type; }
	void setType(WrenDataType type) { data_type = type; }
	std::string getName() { return var_name; }
	bool isPublic() { return is_public; }
	void setEdited(bool value) { edited = value; }
	bool isEdited() { return edited; }

};


struct ImportedMethod
{
	friend class ComponentScript;
	ImportedMethod(std::string name, ImportedVariable::WrenDataType ret_type, std::vector<ImportedVariable> args, WrenHandle* handle);  // for methods
	ImportedMethod(std::string name, WrenHandle* handle) : method_name(name), method_handle(handle) {};  // for setters

private:
	std::string method_name;
	ImportedVariable::WrenDataType return_type = ImportedVariable::WrenDataType::WREN_UNKNOWN;
	WrenHandle* method_handle = nullptr;
	std::vector<ImportedVariable> arg_list;

public:
	std::string getName() { return method_name; };
	ImportedVariable::WrenDataType getReturnType() { return return_type; }
	WrenHandle* getWrenHandle() { return method_handle; };
	std::vector<ImportedVariable> getArgList() { return arg_list; };
};


struct ScriptData
{
	~ScriptData();

	std::string class_name;

	WrenHandle* class_handle = nullptr;

	std::vector<ImportedVariable> vars;
	std::vector<ImportedMethod>  methods;
};
