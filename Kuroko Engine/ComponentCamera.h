#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include "MathGeoLib\Math\float3.h"

class Camera;
class ComponentTransform;

class ComponentCamera : public Component
{
public:
	ComponentCamera(GameObject* parent, Camera* camera);
	~ComponentCamera() {};

	bool Update(float dt);
	void Draw() const;
	Camera* getCamera() const { return camera; };

public:

	bool lock_rotationX = false;
	bool lock_rotationY = false;
	bool lock_rotationZ = false;
	float3 offset = float3::zero;
	bool draw_in_UI = false;
	bool draw_frustum = false;

private:

	Camera* camera = nullptr;
	ComponentTransform* transform = nullptr;

};

#endif