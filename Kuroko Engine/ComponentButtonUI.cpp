#include "ComponentButtonUI.h"



ComponentButtonUI::ComponentButtonUI(GameObject* parent) : Component(parent, UI_BUTTON)
{
}


ComponentButtonUI::~ComponentButtonUI()
{
}

bool ComponentButtonUI::Update(float dt)
{
	return true;
}

void ComponentButtonUI::Draw() const
{
}

void ComponentButtonUI::Save(JSON_Object * config)
{
}
