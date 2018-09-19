#include "PhysBody3D.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
PhysBody3D::PhysBody3D(btRigidBody* body) : body(body)
{

}

// ---------------------------------------------------------
PhysBody3D::~PhysBody3D()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody3D::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody3D::GetTransform(float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}


// ---------------------------------------------------------

vec3 PhysBody3D::GetPos()
{
	vec3 ret;
	btVector3 copy_v = body->getCenterOfMassPosition();
	ret.x = copy_v.getX();
	ret.y = copy_v.getY();
	ret.z = copy_v.getZ();
	return ret;
}

mat3x3 PhysBody3D::GetRotation()
{
	mat3x3 ret;
	float copy_t[16];
	GetTransform(copy_t);

	ret[0] = copy_t[0]; ret[1] = copy_t[1]; ret[2] = copy_t[2];
	ret[3] = copy_t[4]; ret[4] = copy_t[5]; ret[5] = copy_t[6];
	ret[6] = copy_t[8]; ret[7] = copy_t[9]; ret[8] = copy_t[10];

	return ret;
}

// ---------------------------------------------------------
void PhysBody3D::SetTransform(const float* matrix) const
{
	if(body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody3D::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}

void PhysBody3D::SetRotation(mat3x3 rotation)
{
	btTransform t = body->getWorldTransform();
	body->setWorldTransform(t);
}

// ---------------------------------------------------------
void PhysBody3D::SetAsSensor(bool is_sensor)
{
	if(this->is_sensor != is_sensor)
	{
		this->is_sensor = is_sensor;
		if(is_sensor == true)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() &~ btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

// ---------------------------------------------------------
bool PhysBody3D::IsSensor() const
{
	return is_sensor;
}