#ifndef _COMPONENTIMAGEUI_
#define _COMPONENTIMAGEUI_
#include "Component.h"
#include "MathGeoLib/Math/float2.h"

class ComponentRectTransform;

class ComponentImageUI : public Component
{
public:
	ComponentImageUI(GameObject* parent);
	~ComponentImageUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;

private:
	bool texture = false;
	ComponentRectTransform* rectTransform = nullptr;
	float alpha = 1.0f; //not functional yet
	int resourceID = -1;
	float2* texCoords = nullptr;
};

#endif