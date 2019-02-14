#ifndef _COMPONENTTEXTUI_
#define _COMPONENTTEXTUI_
#include "Component.h"

class ComponentRectTransform;

class ComponentTextUI :
	public Component
{
public:
	ComponentTextUI(GameObject* parent);
	~ComponentTextUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:

	ComponentRectTransform * rectTransform = nullptr;
};


#endif