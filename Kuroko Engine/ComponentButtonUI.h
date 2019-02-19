#ifndef _COMPONENTBUTTONUI_
#define _COMPONENTBUTTONUI_

#include "Component.h"

enum ButtonState {
	B_IDLE = 0,
	B_MOUSEOVER,
	B_PRESSED
};

class ComponentRectTransform;
class ComponentButtonUI :public Component
{
public:
	ComponentButtonUI(GameObject* parent);
	~ComponentButtonUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:

	ComponentRectTransform * rectTransform = nullptr;
};

#endif