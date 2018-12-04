#ifndef __SCRIPT_RESOURCE
#define __SCRIPT_RESOURCE
#include "Resource.h"

struct WrenHandle;
class ResourceScript :
	public Resource {
public:
	ResourceScript(resource_deff deff);
	~ResourceScript();

private: 
	WrenHandle* classHandle = nullptr; // Will be called by the scripting module update when game is playing 
};

#endif