#include "ComponentImageUI.h"



ComponentImageUI::ComponentImageUI(GameObject* parent) : Component(parent, UI_IMAGE)
{
	//rectTransform = ...
}


ComponentImageUI::~ComponentImageUI()
{
}

bool ComponentImageUI::Update(float dt)
{
	return true;
}

void ComponentImageUI::Draw() const
{
}

void ComponentImageUI::Save(JSON_Object * config)
{
}
