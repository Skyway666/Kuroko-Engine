#ifndef __SCRIPT_RESOURCE
#define __SCRIPT_RESOURCE
#include "Resource.h"
#include "ModuleScripting.h"

struct WrenHandle;
class ResourceScript :
	public Resource {
public:
	ResourceScript(resource_deff deff);
	~ResourceScript();
	ScriptData* getData() { return data; }

	void Compile();
	void GenerateScript();
	void CleanUp();
private: 
	std::string code;
	std::string class_name;
	ScriptData* data = nullptr;
};

#endif