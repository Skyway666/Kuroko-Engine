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
private: 

	ScriptData* data = nullptr;
};

#endif