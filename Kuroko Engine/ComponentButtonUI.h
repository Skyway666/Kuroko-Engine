#ifndef _COMPONENTBUTTONUI_
#define _COMPONENTBUTTONUI_

#include "Component.h"


class ComponentRectTransform;
class ComponentImageUI;
class ResourceTexture;

enum ButtonState {
	B_IDLE = 0,
	B_MOUSEOVER,
	B_PRESSED
};

class ComponentButtonUI :public Component
{
public:
	ComponentButtonUI(GameObject* parent);
	~ComponentButtonUI();

	bool Update(float dt)override;

	const ResourceTexture* getResourceTexture(ButtonState state);
	void setResourceTexture(ResourceTexture* tex, ButtonState state); 
	void DeassignTexture(ButtonState state);

	inline void setState(ButtonState _state) { state = _state; ChangeGOImage(); };// for debug, may be obsolete
	void ChangeGOImage();

	void Save(JSON_Object* config) override;

	ButtonState state = B_IDLE;

private:

	ComponentRectTransform * rectTransform = nullptr;
	ComponentImageUI* image = nullptr;

	ResourceTexture* idle = nullptr;
	ResourceTexture* hover = nullptr;
	ResourceTexture* pressed = nullptr;
};

#endif