#ifndef _COMPONENTIMAGEUI_
#define _COMPONENTIMAGEUI_
#include "Component.h"

class ComponentRectTransform;

class ComponentImageUI : public Component
{
public:
	ComponentImageUI();
	~ComponentImageUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:

	ComponentRectTransform* rectTransform = nullptr;
};

#endif