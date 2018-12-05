#include "ComponentScript.h"
#include "Wren/wren.hpp"

ImportedMethod::~ImportedMethod()
{
	for (auto it = arg_list.begin(); it != arg_list.end(); it++)
		delete *it;

	arg_list.clear();
}

void* ImportedVariable::GetValue()
{
	switch (data_type)
	{
	case W_BOOL:	return &value_bool; break;
	case W_NUMBER:	return &value_number; break;
	case W_STRING:	return &value_string; break;
	}
	return nullptr;
}

ComponentScript::ComponentScript(GameObject* g_obj, ResourceScript* script) : Component(g_obj, SCRIPT), res_script(script)
{

}

ComponentScript::~ComponentScript()
{
	for (auto it = imported_methods.begin(); it != imported_methods.end(); it++)
		delete *it;

	imported_methods.clear();
	imported_vars.clear();
}

bool ComponentScript::Update(float dt)
{
	/*if (res_script)
		res_script->getHandle()->update(dt);*/

	return true;
}