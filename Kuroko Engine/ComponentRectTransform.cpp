#include "ComponentRectTransform.h"



ComponentRectTransform::ComponentRectTransform(GameObject* parent) : Component(parent, RECTTRANSFORM)
{
}


ComponentRectTransform::~ComponentRectTransform()
{
}

bool ComponentRectTransform::Update(float dt)
{
	return true;
}

void ComponentRectTransform::Draw() const
{
}

void ComponentRectTransform::Save(JSON_Object * config)
{
}
