#ifndef _COMPONENT_SCRIPT
#define _COMPONENT_SCRIPT
#include "Component.h"
#include "ResourceScript.h"
#include <vector>

enum WrenDataType { W_UNKNOWN, W_BOOL, W_STRING, W_NUMBER};

struct ImportedVariable
{
private:
	union  
	{
		bool value_bool = false;
		float value_number;
		const char* value_string;
	};

	std::string var_name;
	WrenDataType data_type = W_UNKNOWN;

public:
	void* GetValue();
	WrenDataType getType()	{ return data_type; }
	std::string getName()	{ return var_name; };

};


struct ImportedMethod
{
	~ImportedMethod();

private:
	uint arg_num = 0;
	std::vector<std::pair<std::string, WrenDataType>*> arg_list;
	std::string method_name;
	WrenDataType return_type = W_UNKNOWN;

public:
	uint getArgNum()				{ return arg_num; };
	std::string getName()			{ return method_name; };
	WrenDataType getReturnType()	{ return return_type; }

	std::vector<std::pair<std::string, WrenDataType>*> getArgList() { return arg_list; };
};


class ComponentScript : public Component 
{
public:
	ComponentScript(GameObject* g_obj, ResourceScript* script = nullptr);
	~ComponentScript();

	bool Update(float dt);

public:

	ResourceScript* res_script = nullptr;
	std::vector<ImportedVariable> imported_vars;
	std::vector<ImportedMethod*> imported_methods;
};


#endif