#include "ComponentInputUI.h"



ComponentInputUI::ComponentInputUI(GameObject* parent) : Component(parent, UI_INPUT)
{
}


ComponentInputUI::~ComponentInputUI()
{
}

bool ComponentInputUI::Update(float dt)
{
	return true;
}

void ComponentInputUI::Draw() const
{
}

void ComponentInputUI::Save(JSON_Object * config)
{
}
