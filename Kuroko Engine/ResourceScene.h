#ifndef __SCENE_RESOURCE
#define __SCENE_RESOURCE
#include "Resource.h"
class ResourceScene :
	public Resource {
public:
	ResourceScene(resource_deff deff);
	~ResourceScene();
	void LoadToMemory();
	void UnloadFromMemory();
};
#endif
