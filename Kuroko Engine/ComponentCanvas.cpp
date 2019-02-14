#include "ComponentCanvas.h"



ComponentCanvas::ComponentCanvas(GameObject* parent) : Component(parent, CANVAS)
{
}


ComponentCanvas::~ComponentCanvas()
{
}

bool ComponentCanvas::Update(float dt)
{
	return true;
}

void ComponentCanvas::Draw() const
{
}

void ComponentCanvas::Save(JSON_Object * config)
{
}
