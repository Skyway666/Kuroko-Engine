#ifndef _COMPONENTCANVAS_
#define _COMPONENTCANVAS_
#include "Component.h"

class ComponentCanvas :	public Component
{
public:
	ComponentCanvas(GameObject* parent);
	~ComponentCanvas();

	bool Update(float dt) override;
	void Draw() const override;
	void Save(JSON_Object* config) override;
};

#endif