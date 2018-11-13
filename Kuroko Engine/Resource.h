#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Globals.h"
enum ResourceType { R_MESH, R_TEXTURE, R_SCENE, R_UNKNOWN };

struct resource_deff {
	uint uuid = 0;
	ResourceType type = R_UNKNOWN;
	std::string binary;
	std::string asset;
};
class Resource {

public:
	Resource(resource_deff deff);
	uint uuid = 0;
	ResourceType type = R_UNKNOWN;
	virtual void LoadToMemory();		// Load resource to memory
	virtual void UnloadFromMemory();    // Unload resource from memory (should put resource pointer to null)
	bool IsLoaded();			// If the resource is loaded to memory

	uint components_used_by = 0; // How many components use this resource
	std::string asset;
protected:


	bool loaded_in_memory = false;
	std::string binary;

};


#endif