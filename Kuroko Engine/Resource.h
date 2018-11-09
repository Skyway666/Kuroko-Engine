#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Globals.h"

enum ResourceType { R_MESH, R_TEXTURE, R_SCENE };
class Resource {

public:
	uint uuid;
	ResourceType type;
	virtual void LoadToMemory();		// Load resource to memory
	virtual void UnloadFromMemory();    // Unload resource from memory (should put resource pointer to null)
	bool IsLoaded();			// If the resource is loaded to memory
protected:

	uint components_used_by = 0; // How many components use this resource
	bool loaded_in_memory;
	std::string file;
	std::string exported_file;


};


#endif