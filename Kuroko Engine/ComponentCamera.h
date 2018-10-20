#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"

class Camera;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, Camera* camera) : Component(parent, C_AABB) {};
	~ComponentCamera();

	bool Update(float dt);

private:

	Camera* camera = nullptr;

};

#endif