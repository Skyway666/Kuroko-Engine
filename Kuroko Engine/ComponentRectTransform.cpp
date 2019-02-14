#include "ComponentRectTransform.h"
#include "Application.h"
#include "ModuleScene.h"

#include "glew-2.1.0\include\GL\glew.h"


ComponentRectTransform::ComponentRectTransform(GameObject* parent) : Component(parent, RECTTRANSFORM)
{
	rect.width = 1.0f;
	rect.height = 1.0f;

	
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
	glLineWidth(1.5f);
	glColor3f(1.0f, 0.0f, 0.0f); // red

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_QUADS);

	glVertex3f(rect.global.x, rect.global.y, 0);
	glVertex3f(rect.global.x + rect.width, rect.global.y, 0);
	glVertex3f(rect.global.x+ rect.width, rect.global.y + rect.height, 0);
	glVertex3f(rect.global.x, rect.global.y + rect.height, 0);

	glEnd();
	App->scene->global_wireframe ?	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE): glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glColor3f(1.0f, 1.0f, 1.0f);
	glLineWidth(1.0f);
}

void ComponentRectTransform::Save(JSON_Object * config)
{
}
