#ifndef _COMPONENT_SCRIPT
#define _COMPONENT_SCRIPT
#include "Component.h"
#include "ResourceScript.h"
#include <vector>


class ComponentScript : public Component 
{
public:
	ComponentScript(GameObject* g_obj, uint script_resource_uuid = 0);
	~ComponentScript();

	bool Update(float dt);

public:

	uint script_resource_uuid = 0;
};


#endif