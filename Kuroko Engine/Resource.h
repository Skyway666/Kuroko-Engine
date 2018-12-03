#ifndef __RESOURCE__
#define __RESOURCE__

#include <string>
#include "Globals.h"
enum ResourceType { R_MESH, R_TEXTURE, R_SCENE, R_SCRIPT, R_UNKNOWN };
enum ResourceUpdateActions{R_UPDATE, R_DELETE, R_CREATE, R_NOTHING};

struct resource_deff {
	resource_deff() {}
	resource_deff(uint uuid, ResourceType type, std::string binary, std::string asset) {
		this->uuid = uuid;
		this->type = type;
		this->binary = binary;
		this->asset = asset;
	}

	void set(uint uuid, ResourceType type, std::string binary, std::string asset) {
		this->uuid = uuid;
		this->type = type;
		this->binary = binary;
		this->asset = asset;
	}
	uint uuid = 0;
	ResourceType type = R_UNKNOWN;
	std::string binary;
	std::string asset;
	ResourceUpdateActions requested_update = R_NOTHING;
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
	std::string binary;
	std::string asset;
protected:
	bool loaded_in_memory = false;
};


#endif