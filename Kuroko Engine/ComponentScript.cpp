#include "ComponentScript.h"
#include "Wren/wren.hpp"


ComponentScript::ComponentScript(GameObject* g_obj, uint script_resource_uuid) : Component(g_obj, SCRIPT), script_resource_uuid(script_resource_uuid)
{

}

ComponentScript::~ComponentScript()
{
}

bool ComponentScript::Update(float dt)
{
	/*if (res_script)
		res_script->getHandle()->update(dt);*/

	return true;
}