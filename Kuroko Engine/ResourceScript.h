#pragma once
#include "Resource.h"

struct WrenHandle;
class ResourceScript :
	public Resource {
public:
	ResourceScript(resource_deff deff);
	~ResourceScript();

private: 
	WrenHandle* classHandle; // Will be called by the scripting module update when game is playing 
};

