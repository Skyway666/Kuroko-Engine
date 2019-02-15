#ifndef _COMPONENTRECTTRANSFORM_
#define _COMPONENTRECTTRANSFORM_

#include "Component.h"
#include "MathGeoLib/Math/float2.h"
#include "MathGeoLib/Math/float3.h"

class RectTransform {
public:
	float2 local = float2::zero;
	float2 global = float2::zero;
	float width = 0.0f;
	float height = 0.0f;
};

class ComponentRectTransform :	public Component
{
public:
	ComponentRectTransform(GameObject* parent);
	~ComponentRectTransform();
	
	bool Update(float dt) override;
	void Draw() const override;
	void Save(JSON_Object* config) override;

	inline void setLocalPos(float2 pos);
	void setGlobalPos(float2 pos);
	inline void setWidth(float width);
	inline void setHeight(float height);

	inline const float2 getLocalPos() { return rect.local; }
	inline const float2 getGlobalPos() { return rect.global; }
	inline const float getWidth() { return rect.width; }
	inline const float getHeight() { return rect.height; }
private:
	RectTransform rect;
};

#endif