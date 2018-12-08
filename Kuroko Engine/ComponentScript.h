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

	// Assign resource and update editor variables
	void assignScriptResource(uint new_script_uuid);
	
	void CleanSetterHandlers();
	void GenerateSetterHandlers();

	bool Update(float dt);

	// The handler for the editor variable setter is in the same index in the setter_handlers array
	std::vector<ImportedVariable> editor_variables;
	std::vector<WrenHandle*> setter_handlers;
public:

	uint script_resource_uuid = 0;
	void assignEditorValues(ResourceScript* script_res);
};


#endif