#include "ComponentScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleResourcesManager.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Applog.h"


ComponentScript::ComponentScript(GameObject* g_obj, uint resource_uuid) : Component(g_obj, SCRIPT)
{
	if(resource_uuid != 0)
		assignScriptResource(resource_uuid);
}

ComponentScript::ComponentScript(JSON_Object * deff, GameObject * parent): Component(parent, SCRIPT) {

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

	ResourceScript* script_r = (ResourceScript*)App->resources->getResource(script_resource_uuid);

	if (!script_r) {
		app_log->AddLog("Couldn't load script, resource not found");
		script_name = "Unknown";
		instance_data = nullptr;
		return;
	}
	ScriptData* base_script_data = script_r->getData();
	script_name = script_r->class_name.c_str();

	if (!base_script_data) {
		app_log->AddLog("Couldn't load script, resource is invalid");
		instance_data = nullptr;
		return;
	}

	instance_data = new ScriptData();

	instance_data->class_name = base_script_data->class_name;
	instance_data->vars = base_script_data->vars;
	instance_data->methods = base_script_data->methods;
	instance_data->class_handle = App->scripting->GetHandlerToClass(instance_data->class_name.c_str(), instance_data->class_name.c_str());

	LinkScriptToObject();

	App->scripting->loaded_instances.push_back(instance_data);

}
void ComponentScript::LinkScriptToObject() {

	WrenVM* vm = App->scripting->vm;
	WrenHandle* class_handle = instance_data->class_handle;
	wrenEnsureSlots(vm, 2); // One for class and other for GO

	// Set instance
	wrenSetSlotHandle(vm, 0, class_handle);
	// Set argument
	wrenSetSlotDouble(vm, 1, getParent()->getUUID());
	// Call setter
	wrenCall(vm, App->scripting->base_signatures.at("gameObject=(_)"));

}



void ComponentScript::CleanUp() {
	App->scripting->loaded_instances.remove(instance_data);
	delete instance_data;
}

void ComponentScript::Save(JSON_Object * config) {

	json_object_set_string(config, "type", "script");
	json_object_set_number(config, "script_resource_uuid", script_resource_uuid); // Gives us all we need but the variables

	// Store the variables
	if (!instance_data) {
		return;
	}
	// Create and fill array
	JSON_Value* vars_array = json_value_init_array();
	for (auto it = instance_data->vars.begin(); it != instance_data->vars.end(); it++) {
		JSON_Value* var = json_value_init_object();

		json_object_set_string(json_object(var), "name", (*it).getName().c_str());
		switch ((*it).getType()) {
		case ImportedVariable::WREN_NUMBER:
			json_object_set_string(json_object(var), "type", "number");
			json_object_set_number(json_object(var), "value", (*it).GetValue().value_number);
			break;
		case ImportedVariable::WREN_BOOL:
			json_object_set_string(json_object(var), "type", "bool");
			json_object_set_boolean(json_object(var), "value", (*it).GetValue().value_bool);
			break;
		case ImportedVariable::WREN_STRING:
			json_object_set_string(json_object(var), "type", "string");
			json_object_set_string(json_object(var), "value", (*it).GetValue().value_string);
			break;
		}

		json_object_set_boolean(json_object(var), "public", (*it).isPublic());
		json_object_set_boolean(json_object(var), "edited", (*it).isEdited());
		json_object_set_boolean(json_object(var), "tupe_forced", (*it).isTypeForced());

		json_array_append_value(json_array(vars_array), var);
	}

	json_object_set_value(config, "variables", vars_array);
}


ComponentScript::~ComponentScript()
{
	CleanUp();
}

