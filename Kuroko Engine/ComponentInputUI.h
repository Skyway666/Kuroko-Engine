#ifndef _COMPONENTINPUTUI_
#define _COMPONENTINPUTUI_

#include "Component.h"

class ComponentRectTransform;

class ComponentInputUI :public Component
{
public:
	ComponentInputUI(GameObject* parent);
	~ComponentInputUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:

	ComponentRectTransform * rectTransform = nullptr;
};


#endif