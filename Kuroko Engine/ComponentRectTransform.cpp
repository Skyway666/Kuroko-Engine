#include "ComponentRectTransform.h"



ComponentRectTransform::ComponentRectTransform() : Component(parent, UI_IMAGE)
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
