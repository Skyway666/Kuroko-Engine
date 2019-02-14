#ifndef _COMPONENTCHECKBOXUI_
#define _COMPONENTCHECKBOXUI_

#include "Component.h"

class ComponentRectTransform;

class ComponentCheckBoxUI :
	public Component
{
public:
	ComponentCheckBoxUI(GameObject* parent);
	~ComponentCheckBoxUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:

	ComponentRectTransform * rectTransform = nullptr;
};

#endif