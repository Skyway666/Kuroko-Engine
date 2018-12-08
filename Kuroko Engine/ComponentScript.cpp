#include "ComponentScript.h"
#include "Wren/wren.hpp"
#include "Application.h"
#include "ModuleResourcesManager.h"
#include "ModuleInput.h"


ComponentScript::ComponentScript(GameObject* g_obj, uint script_resource_uuid) : Component(g_obj, SCRIPT)
{
	assignScriptResource(script_resource_uuid);
}

ComponentScript::~ComponentScript()
{

}

void ComponentScript::assignScriptResource(uint new_script_uuid) {

	// Clean previous handlers if any
	CleanSetterHandlers();

	// Assign resource
	script_resource_uuid = new_script_uuid;
	App->resources->assignResource(script_resource_uuid);

	// Copy script variables
	editor_variables = ((ResourceScript*)App->resources->getResource(script_resource_uuid))->getData()->vars;

	// Generate handlers for the variable setters
	GenerateSetterHandlers();
}

void ComponentScript::CleanSetterHandlers() {
	for (int i = 0; i < setter_handlers.size(); i++) {
		wrenReleaseHandle(App->scripting->vm, setter_handlers[i]);
	}
	setter_handlers.clear();
}

void ComponentScript::GenerateSetterHandlers() {

	for (int i = 0; i < setter_handlers.size(); i++) {
		std::string setter_signature = editor_variables[i].getName();
		setter_signature += "=(_)";
		setter_handlers[i] = wrenMakeCallHandle(App->scripting->vm, setter_signature.c_str());
	}
}

void ComponentScript::assignEditorValues(ResourceScript* script_res) {

	// Get all the variables that need to be set before update
	for (int i = 0; i < editor_variables.size(); i++) {
		wrenSetSlotHandle(App->scripting->vm, 0, script_res->getData()->class_handle); // Set class
		wrenSetSlotDouble(App->scripting->vm, 1, i/*editor_variables[i].GetValue()*/); // Set Value
		wrenCall(App->scripting->vm, setter_handlers[i]); // Call the setter

	}
}

bool ComponentScript::Update(float dt)
{

	ResourceScript* script_res = (ResourceScript*)App->resources->getResource(script_resource_uuid);

	assignEditorValues(script_res);
	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) {
			wrenSetSlotHandle(App->scripting->vm, 0, script_res->getData()->class_handle);
			wrenCall(App->scripting->vm, App->scripting->base_signatures.at("Update()"));
	}

	return true;
}