#ifndef _COMPONENTRECTTRANSFORM_
#define _COMPONENTRECTTRANSFORM_

#include "Component.h"
#include "MathGeoLib/Math/float2.h"

class ComponentRectTransform :	public Component
{
public:
	ComponentRectTransform(GameObject* parent);
	~ComponentRectTransform();

	float2 local = float2::zero;
	float2 global = float2::zero;
	float width = 0.0f;
	float height = 0.0f;

	bool Update(float dt) override;
	void Draw() const override;
	void Save(JSON_Object* config) override;
};

#endif