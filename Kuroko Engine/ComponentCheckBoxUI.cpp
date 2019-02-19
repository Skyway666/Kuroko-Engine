#include "ComponentCheckBoxUI.h"
#include "ComponentImageUI.h"
#include "GameObject.h"


ComponentCheckBoxUI::ComponentCheckBoxUI(GameObject* parent) : Component(parent, UI_CHECKBOX)
{
	image = (ComponentImageUI*) parent->getComponent(UI_IMAGE);
}


ComponentCheckBoxUI::~ComponentCheckBoxUI()
{
	image = nullptr;
	pressed = nullptr;
	idle = nullptr;
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


void ComponentCheckBoxUI::ChangeGOImage()
{
	switch (state)
	{
	case IDLE:
		image->setResourceTexture(idle);
		break;
	case PRESSED:
		image->setResourceTexture(pressed);
		break;	
	}
}

bool ComponentCheckBoxUI::isMouseOver()
{
	return false;
}
