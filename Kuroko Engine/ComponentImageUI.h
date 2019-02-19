#ifndef _COMPONENTIMAGEUI_
#define _COMPONENTIMAGEUI_
#include "Component.h"
#include "MathGeoLib/Math/float2.h"

class ComponentRectTransform;
class ResourceTexture;

class ComponentImageUI : public Component
{
public:
	ComponentImageUI(GameObject* parent);
	~ComponentImageUI();

	bool Update(float dt)override;
	void Draw() const override;

	void Save(JSON_Object* config) override;
	
	inline const ResourceTexture* getResourceTexture() { return texture; }
	inline void setResourceTexture(ResourceTexture* tex) { texture = tex; }
	inline void DeassignTexture() { texture = nullptr; }


private:
	ComponentRectTransform* rectTransform = nullptr;
	
	float alpha = 1.0f; //not functional yet
	ResourceTexture* texture = nullptr;
	float2* texCoords = nullptr;

};

#endif