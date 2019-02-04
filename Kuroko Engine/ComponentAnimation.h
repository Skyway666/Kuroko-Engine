#ifndef _COMPONENT_ANIMATION
#define _COMPONENT_ANIMATION

#include "Component.h"
//TMP - substitute by Animation.h
#include "Globals.h"
#include <map>

class ComponentAnimation : public Component
{
public:
	ComponentAnimation(GameObject* gameobject) : Component(gameobject, ANIMATION) {};   // empty constructor
	ComponentAnimation(JSON_Object* deff, GameObject* parent);
	~ComponentAnimation();

	bool Update(float dt);

	void Save(JSON_Object* config);

	bool Finished() const { return false; }

public:

	bool loop = false;
	float speed = 1.0f;

private:

	uint animation_resource_uuid = 0;

	float animTime = 0.0f;

	std::map<uint, uint> bones;
};

#endif // !_COMPONENT_ANIMATION