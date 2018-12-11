#ifndef _COMPONENT_SCRIPT
#define _COMPONENT_SCRIPT
#include "Component.h"
#include "ResourceScript.h"
#include "ScriptData.h"
#include <vector>

class ComponentScript : public Component 
{
public:
	ComponentScript(GameObject* g_obj, uint resource_uuid = 0);
	~ComponentScript();

	uint getResourceUUID() { return script_resource_uuid; }; 
	void assignScriptResource(uint resource_uuid);
	void LoadResource();
	void CleanUp();

	ScriptData* instance_data = nullptr;
private:

	uint script_resource_uuid = 0;
};


#endif