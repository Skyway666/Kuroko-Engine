#include "ComponentTextUI.h"



ComponentTextUI::ComponentTextUI(GameObject* parent) : Component(parent, UI_TEXT)
{
}


ComponentTextUI::~ComponentTextUI()
{
}

bool ComponentTextUI::Update(float dt)
{
	return true;
}

void ComponentTextUI::Draw() const
{
}

void ComponentTextUI::Save(JSON_Object * config)
{
}
