#include "ComponentCheckBoxUI.h"



ComponentCheckBoxUI::ComponentCheckBoxUI(GameObject* parent) : Component(parent, UI_CHECKBOX)
{
}


ComponentCheckBoxUI::~ComponentCheckBoxUI()
{
}

bool ComponentCheckBoxUI::Update(float dt)
{
	return true;
}

void ComponentCheckBoxUI::Draw() const
{
}

void ComponentCheckBoxUI::Save(JSON_Object * config)
{
}
