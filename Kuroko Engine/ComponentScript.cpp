#include "ComponentScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleResourcesManager.h"
#include "ModuleInput.h"


ComponentScript::ComponentScript(GameObject* g_obj, uint resource_uuid) : Component(g_obj, SCRIPT)
{
	if(resource_uuid != 0)
		assignScriptResource(resource_uuid);
}

void ComponentScript::assignScriptResource(uint resource_uuid)
{
	if (instance_data)
	{
		CleanUp();
	}

	script_resource_uuid = resource_uuid;
	App->resources->assignResource(script_resource_uuid);

	LoadResource();
}

void ComponentScript::LoadResource() {

	ScriptData* base_script_data = (((ResourceScript*)App->resources->getResource(script_resource_uuid))->getData());

	instance_data = new ScriptData();

	instance_data->class_name = base_script_data->class_name;
	instance_data->vars = base_script_data->vars;
	instance_data->methods = base_script_data->methods;
	instance_data->class_handle = App->scripting->GetHandlerToClass(instance_data->class_name.c_str(), instance_data->class_name.c_str());

	App->scripting->loaded_instances.push_back(instance_data);

}

void ComponentScript::CleanUp() {
	App->scripting->loaded_instances.remove(instance_data);
	delete instance_data;
}

ComponentScript::~ComponentScript()
{
	CleanUp();
}

