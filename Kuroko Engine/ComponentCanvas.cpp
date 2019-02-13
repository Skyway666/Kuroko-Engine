#include "ComponentCanvas.h"



ComponentCanvas::ComponentCanvas() : Component(parent, CANVAS)
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
