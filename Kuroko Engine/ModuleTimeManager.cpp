#include "ModuleTimeManager.h"





ModuleTimeManager::ModuleTimeManager(Application * app, bool start_enabled): Module(app, start_enabled)
{
	name = "time_manager";
}



ModuleTimeManager::~ModuleTimeManager()
{

}

bool ModuleTimeManager::Init(const JSON_Object * config)
{
	return true;
}

update_status ModuleTimeManager::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleTimeManager::PostUpdate(float dt)
{
	return update_status();
}

bool ModuleTimeManager::CleanUp()
{
	return false;
}
