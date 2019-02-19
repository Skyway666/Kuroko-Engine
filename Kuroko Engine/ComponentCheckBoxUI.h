#ifndef _COMPONENTCHECKBOXUI_
#define _COMPONENTCHECKBOXUI_

#include "Component.h"

class ComponentRectTransform;
class ComponentImageUI;
class ResourceTexture;

enum CheckBoxState {
	IDLE = 0,
	PRESSED
};

class ComponentCheckBoxUI :
	public Component
{
public:
	ComponentCheckBoxUI(GameObject* parent);
	~ComponentCheckBoxUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

	inline const ResourceTexture* getResourceTexture(CheckBoxState state) { return state == IDLE ? idle : pressed; }
	inline void setResourceTexture(ResourceTexture* tex, CheckBoxState state) { state == IDLE ? idle = tex : pressed = tex; ChangeGOImage(); }
	inline void DeassignTexture(CheckBoxState state) { state == IDLE ? idle = nullptr : pressed = nullptr; ChangeGOImage(); }

	inline void Press() { state == IDLE ? state = PRESSED : state = IDLE; ChangeGOImage(); } // for debug may be obsolete
	inline const bool isPressed() { return state == PRESSED; }// for debug may be obsolete

	CheckBoxState state = IDLE;

private:
	void ChangeGOImage();
	bool isMouseOver();
	ComponentRectTransform * rectTransform = nullptr;
	ComponentImageUI* image = nullptr;

	ResourceTexture* idle = nullptr;
	ResourceTexture* pressed = nullptr;
};

#endif